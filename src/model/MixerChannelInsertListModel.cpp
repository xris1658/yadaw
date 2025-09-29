#include "MixerChannelInsertListModel.hpp"

#include "audio/plugin/IAudioPlugin.hpp"
#include "audio/engine/AudioDeviceGraphProcess.hpp"
#include "audio/engine/MultiInputDeviceWithPDC.hpp"
#include "audio/util/CLAPHelper.hpp"
#include "audio/util/VST3Helper.hpp"
#include "controller/AudioEngineController.hpp"
#include "controller/LibraryPluginMap.hpp"
#include "controller/PluginPositionMap.hpp"
#include "controller/PluginController.hpp"
#include "controller/PluginIOConfigUpdatedCallback.hpp"
#include "controller/PluginLatencyUpdatedCallback.hpp"
#include "controller/PluginListController.hpp"
#include "controller/PluginWindowController.hpp"
#include "controller/VST3PluginPool.hpp"
#include "event/EventBase.hpp"
#include "dao/PluginTable.hpp"
#include "util/QmlUtil.hpp"

#include <new>

namespace YADAW::Model
{
using YADAW::Audio::Engine::AudioDeviceProcess;
using YADAW::Audio::Engine::MultiInputDeviceWithPDC;
using YADAW::Audio::Plugin::IAudioPlugin;

MixerChannelInsertListModel::MixerChannelInsertListModel(
    YADAW::Audio::Mixer::Inserts& inserts,
    YADAW::Audio::Mixer::Mixer::ChannelListType type,
    std::uint32_t channelIndex,
    bool isPreFaderInsert,
    std::uint32_t insertsIndex,
    QObject* parent):
    IMixerChannelInsertListModel(parent),
    inserts_(&inserts),
    channelIndex_(channelIndex),
    isPreFaderInsert_(isPreFaderInsert),
    insertsIndex_(insertsIndex),
    fillPluginContextCallback_(&Impl::blankFillPluginContext)
{
    // If this step is missing, then the object ownership will be implicitly set
    // to `QJSEngine::ObjectOwnership::JavaScriptOwnership` when the object is
    // used in QML (e.g. in `MainWindow.qml`), allowing the object to be
    // (incorrectly) destroyed by `deleteLater()` calls!
    // - From what I can tell, the member function that allows this mess is
    //   `QQmlData::setImplicitDestructible()` in
    //   - `include/QtQml/<version>/QtQml/private/qqmldata_p.h`
    //     in binary includes, or
    //   - `qtdeclarative/src/qml/qml/qqmldata_p.h` in sources.
    //   Thank you QML JS Engine for being a troublemaker!
    // - I haven't checked source of QML JS Engine, so I don't really know how
    //   the implicit ownership update happens.
    // - I can make `deleteLater()` not working by filtering out events of type
    //   `QEvent::Type::DeferredDelete`, but the process might be a little more
    //   complex. Maybe it's a better idea to set ownership in the constructor?
    //
    YADAW::Util::setCppOwnership(*this);
}

MixerChannelInsertListModel::~MixerChannelInsertListModel()
{}

int MixerChannelInsertListModel::itemCount() const
{
    return inserts_->insertCount();
}

int MixerChannelInsertListModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant MixerChannelInsertListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& graph = inserts_->graph();
        const auto& node = *(inserts_->insertNodeAt(row));
        const auto& nodeData = graph.getNodeData(node);
        auto device = nodeData.process.device();
        if(device->audioInputGroupCount() > 1)
        {
            auto multiInput = dynamic_cast<const MultiInputDeviceWithPDC*>(device);
            if(multiInput)
            {
                device = multiInput->process().device();
            }
        }
        auto& context = inserts_->insertContextAt(row)->get();
        auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context.get());
        auto& pluginContextUserData = *static_cast<YADAW::Model::PluginContextUserData*>(pluginContext.userData.get());
        switch(role)
        {
        case Role::Name:
        {
            return QVariant::fromValue<QString>(pluginContextUserData.name);
        }
        case Role::Bypassed:
        {
            return QVariant::fromValue<bool>(*inserts_->insertBypassed(row));
        }
        case Role::AudioInputs:
        {
            return QVariant::fromValue<QObject*>(&pluginContextUserData.audioInputs);
        }
        case Role::AudioOutputs:
        {
            return QVariant::fromValue<QObject*>(&pluginContextUserData.audioOutputs);
        }
        case Role::AudioAuxInputs:
        {
            return QVariant::fromValue<QObject*>(&pluginContextUserData.getAudioAuxInputs());
        }
        case Role::AudioAuxOutputs:
        {
            return QVariant::fromValue<QObject*>(&pluginContextUserData.getAudioAuxOutputs());
        }
        case Role::HasUI:
        {
            auto ret = false;
            auto plugin = dynamic_cast<const IAudioPlugin*>(device);
            if(plugin)
            {
                ret = plugin->gui();
            }
            return QVariant::fromValue<bool>(ret);
        }
        case Role::WindowVisible:
        {
            return QVariant::fromValue<bool>(
                pluginContext.editor && pluginContext.editor->isVisible()
            );
        }
        case Role::GenericEditorVisible:
        {
            return QVariant::fromValue<bool>(
                pluginContext.genericEditor && pluginContext.genericEditor->isVisible()
            );
        }
        case Role::Latency:
        {
            return QVariant::fromValue(device->latencyInSamples());
        }
        }
    }
    return {};
}

bool MixerChannelInsertListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto& graph = inserts_->graph();
        auto node = *(inserts_->insertNodeAt(row));
        auto& nodeData = graph.getNodeData(node);
        auto device = nodeData.process.device();
        if(device->audioInputGroupCount() > 1)
        {
            auto multiInput = dynamic_cast<MultiInputDeviceWithPDC*>(device);
            if(multiInput)
            {
                device = multiInput->process().device();
            }
        }
        auto& context = inserts_->insertContextAt(row)->get();
        auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context.get());
        auto& pluginContextUserData = *static_cast<YADAW::Model::PluginContextUserData*>(pluginContext.userData.get());
        switch(role)
        {
        case Role::Bypassed:
        {
            inserts_->setBypassed(row, value.value<bool>());
            dataChanged(index, index, {Role::Bypassed});
            return true;
        }
        case Role::WindowVisible:
        {
            if(auto window = pluginContext.editor)
            {
                window->setVisible(value.value<bool>());
                return true;
            }
            return false;
        }
        case Role::GenericEditorVisible:
        {
            pluginContext.genericEditor->setVisible(value.value<bool>());
            return true;
        }
        }
    }
    return false;
}

bool YADAW::Model::MixerChannelInsertListModel::insert(int position, int pluginId)
{
    auto ret = false;
    if(position >= 0 && position <= inserts_->insertCount())
    {
        const auto& pluginInfo = YADAW::DAO::selectPluginById(pluginId);
        auto& group = inserts_->graph().getNodeData(
            inserts_->inNode()
        ).process.device()->audioOutputGroupAt(
            inserts_->inChannelGroupIndex()
        )->get();
        YADAW::Controller::InitPluginArgs initPluginArgs {
            {group.type(), group.channelCount()},
            {group.type(), group.channelCount()}
        };
        auto optionalPluginContext = YADAW::Controller::createPluginWithContext(
            pluginInfo.path,
            static_cast<YADAW::DAO::PluginFormat>(pluginInfo.format),
            pluginInfo.uid,
            [this, &initPluginArgs](YADAW::Controller::PluginContext& context)
            {
                return fillPluginContextCallback_(context, initPluginArgs);
            }
        );
        if(optionalPluginContext.has_value())
        {
            ret = true;
            auto& pluginContext = *optionalPluginContext;
            pluginContext.position.position = YADAW::Controller::PluginPosition::InChannelPosition::Insert;
            pluginContext.position.index = position;
            pluginContext.position.model = this;
            auto uPtrContextUserData = YADAW::Util::createPMRUniquePtr(
                std::make_unique<PluginContextUserData>(
                    pluginContext, pluginInfo.name
                )
            );
            auto contextUserData = uPtrContextUserData.get();
            pluginContext.userData = std::move(uPtrContextUserData);
            auto& plugin = pluginContext.pluginInstance.plugin()->get();
            auto& userData = *static_cast<PluginContextUserData*>(
                pluginContext.userData.get()
            );
            if(auto pluginWindow = pluginContext.editor)
            {
                pluginWindow->setTitle(pluginInfo.name);
                userData.pluginWindowConnection = QObject::connect(
                    pluginWindow, &QWindow::visibleChanged,
                    [this, position](bool visible)
                    {
                        dataChanged(this->index(position), this->index(position),
                            {Role::WindowVisible}
                        );
                    }
                );
            }
            auto genericEditor = pluginContext.genericEditor;
            genericEditor->setTitle(pluginInfo.name);
            userData.genericEditorWindowConnection = QObject::connect(
                genericEditor, &QWindow::visibleChanged,
                [this, position](bool visible)
                {
                    dataChanged(this->index(position), this->index(position),
                        {Role::GenericEditorVisible}
                    );
                }
            );
            genericEditor->setProperty(
                "parameterListModel",
                QVariant::fromValue<QObject*>(&userData.paramListModel)
            );
            auto node = pluginContext.node;
            auto mixerContext = YADAW::Util::createPMRUniquePtr(
                std::make_unique<YADAW::Controller::PluginContext>(
                    std::move(pluginContext)
                )
            );
            YADAW::Controller::appPluginContexts().emplace(mixerContext.get());
            beginInsertRows(QModelIndex(), position, position);
            inserts_->insert(
                node, std::move(mixerContext), position
            );
            contextUserData->initAuxModels(
                inserts_->insertInputChannelGroupIndexAt(position),
                inserts_->insertOutputChannelGroupIndexAt(position)
            );
            endInsertRows();
            updateInsertConnections(position + 1);
        }
    }
    YADAW::Controller::pluginWindows.pluginWindow = nullptr;
    YADAW::Controller::pluginWindows.genericEditorWindow = nullptr;
    return ret;
}

bool YADAW::Model::MixerChannelInsertListModel::append(int pluginId)
{
    return insert(inserts_->insertCount(), pluginId);
}

bool MixerChannelInsertListModel::remove(int position, int removeCount)
{
    auto ret = false;
    if(position >= 0 && position < inserts_->insertCount()
        && position + removeCount >= 0
        && position + removeCount <= inserts_->insertCount())
    {
        ret = true;
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        std::vector<YADAW::Audio::Mixer::Context> contexts;
        std::vector<std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC>> multiInputDevices;
        auto contextCount = 0;
        auto multiInputDeviceCount = 0;
        auto& audioEngine = YADAW::Controller::AudioEngine::appAudioEngine();
        auto& graph = audioEngine.mixer().graph();
        FOR_RANGE(i, position, position + removeCount)
        {
            contextCount += static_cast<bool>(inserts_->insertContextAt(i)->get().get());
            auto device = graph.graph().getNodeData(*inserts_->insertNodeAt(i)).process.device();
            multiInputDeviceCount += (device->audioInputGroupCount() > 1);
        }
        contexts.reserve(contextCount);
        multiInputDevices.reserve(multiInputDeviceCount);
        inserts_->detachContexts(
            [&contexts](YADAW::Audio::Mixer::Context&& context)
            {
                if(context)
                {
                    contexts.emplace_back(std::move(context));
                }
            },
            position, removeCount
        );
        std::vector<ade::NodeHandle> nodes;
        nodes.reserve(removeCount);
        FOR_RANGE(i, position, position + removeCount)
        {
            nodes.emplace_back(*inserts_->insertNodeAt(i));
        }
        inserts_->remove(position, removeCount);
        for(auto node: nodes)
        {
            if(auto multiInputDevice = graph.removeNode(node))
            {
                multiInputDevices.emplace_back(std::move(multiInputDevice));
            }
        }
        audioEngine.insertsNodeRemovedCallback(*inserts_);
        multiInputDevices.clear();
        for(auto& context: contexts)
        {
            auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context.get());
            auto& plugin = pluginContext.pluginInstance.plugin()->get();
            YADAW::Controller::appPluginContexts().erase(&pluginContext);
            if(auto window = pluginContext.editor)
            {
                plugin.gui()->detachWithWindow();
                window->setProperty("destroyingPlugin", QVariant::fromValue(true));
                delete window;
            }
            auto genericEditor = pluginContext.genericEditor;
            genericEditor->setProperty("destroyingPlugin", QVariant::fromValue(true));
            delete genericEditor;
            auto& engine = YADAW::Controller::AudioEngine::appAudioEngine();
            if(auto format = plugin.format(); format == YADAW::Audio::Plugin::PluginFormat::VST3)
            {
                auto& pool = YADAW::Controller::appVST3PluginPool();
                pool.erase(
                    static_cast<YADAW::Audio::Plugin::VST3Plugin*>(&plugin)
                );
                engine.vst3PluginPool().updateAndGetOld(
                    std::make_unique<YADAW::Controller::VST3PluginPoolVector>(
                        YADAW::Controller::createPoolVector(pool)
                    ),
                    engine.running()
                );
            }
            else if(format == YADAW::Audio::Plugin::PluginFormat::CLAP)
            {
                // TODO: Decouple with `appAudioEngine`
                //   Since `CLAPPlugin::stopProcessing` has to be called in the
                //   audio thread, I have to couple this model with
                //   `appAudioEngine`.
                auto* clapPlugin = static_cast<YADAW::Audio::Plugin::CLAPPlugin*>(&plugin);
                engine.clapPluginToSetProcess().updateAndGetOld(
                    std::make_unique<YADAW::Controller::CLAPPluginToSetProcessVector>(
                        1, std::make_pair(clapPlugin, false)
                    ),
                    engine.running()
                );
                auto& pool = YADAW::Controller::appCLAPPluginPool();
                pool.erase(
                    static_cast<YADAW::Audio::Plugin::CLAPPlugin*>(&plugin)
                );
                engine.clapPluginPool().updateAndGetOld(
                    std::make_unique<YADAW::Controller::CLAPPluginPoolVector>(
                        YADAW::Controller::createPoolVector(pool)
                    ),
                    engine.running()
                );
            }
        }
        endRemoveRows();
        updateInsertConnections(position);
    }
    return ret;
}

bool MixerChannelInsertListModel::replace(int position, int pluginId)
{
    return false;
}

void MixerChannelInsertListModel::clear()
{
    remove(0, inserts_->insertCount());
}

std::uint32_t MixerChannelInsertListModel::channelIndex() const
{
    return channelIndex_;
}

void MixerChannelInsertListModel::setChannelIndex(std::uint32_t channelIndex)
{
    if(channelIndex_ != channelIndex)
    {
        channelIndex_ = channelIndex;
        channelIndexChanged();
    }
}

void MixerChannelInsertListModel::setPreFaderInsert(bool isPreFaderInsert)
{
    isPreFaderInsert_ = isPreFaderInsert;
}

void MixerChannelInsertListModel::setInsertsIndex(std::uint32_t insertsIndex)
{
    insertsIndex_ = insertsIndex;
}

void MixerChannelInsertListModel::latencyUpdated(std::uint32_t index) const
{
    if(index < itemCount())
    {
        const_cast<MixerChannelInsertListModel*>(this)->dataChanged(
            this->index(index),
            this->index(index),
            {Role::Latency}
        );
    }
}

void MixerChannelInsertListModel::updateIOConfig(std::uint32_t index)
{
    if(index < itemCount())
    {
        auto& audioEngine = YADAW::Controller::AudioEngine::appAudioEngine();
        auto& mixer = audioEngine.mixer();
        auto& graphWithPDC = mixer.graph();
        auto& graph = graphWithPDC.graph();
        auto node = *(inserts_->insertNodeAt(index));
        struct Point
        {
            std::uint32_t fromChannelGroupIndex;
            std::uint32_t toChannelGroupIndex;
            ade::NodeHandle node;
        };
        std::vector<Point> inputPoints;
        std::vector<Point> outputPoints;
        auto inEdges = node->inEdges();
        auto size = inEdges.size();
        inputPoints.reserve(size);
        for(const auto& edgeHandle: inEdges)
        {
            const auto& edgeData = graph.getEdgeData(edgeHandle);
            inputPoints.emplace_back(
                Point {
                    edgeData.fromChannel,
                    edgeData.toChannel,
                    edgeHandle->srcNode()
                }
            );
        }
        auto outEdges = node->outEdges();
        size = outEdges.size();
        outputPoints.reserve(size);
        for(const auto& edgeHandle: outEdges)
        {
            const auto& edgeData = graph.getEdgeData(edgeHandle);
            outputPoints.emplace_back(
                Point {
                    edgeData.fromChannel,
                    edgeData.toChannel,
                    edgeHandle->dstNode()
                }
            );
        }
        auto process = graph.getNodeData(node).process;
        auto device = process.device();
        if(device->audioInputGroupCount() > 1)
        {
            auto multiInput = static_cast<MultiInputDeviceWithPDC*>(device);
            process = multiInput->process();
            device = process.device();
        }
        auto context = YADAW::Util::createUniquePtr(nullptr);
        inserts_->detachContexts(
            [&context](YADAW::Audio::Mixer::Context&& insertContext)
            {
                context = std::move(insertContext);
            },
            index, 1
        );
        auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context.get());
        auto& pluginContextUserData = *static_cast<YADAW::Model::PluginContextUserData*>(pluginContext.userData.get());
        inserts_->remove(index);
        auto name = pluginContextUserData.name;
        {
            auto disposingDevice = graphWithPDC.removeNode(node);
            YADAW::Controller::AudioEngine::insertsNodeRemovedCallback(*inserts_);
        }
        auto plugin = static_cast<YADAW::Audio::Plugin::IAudioPlugin*>(device);
        auto status = plugin->status();
        // TODO: Start/stop processing plugins on audio thread if needed
        plugin->stopProcessing();
        plugin->deactivate();
        pluginContextUserData.audioInputs.reset();
        pluginContextUserData.audioOutputs.reset();
        if(status >= IAudioPlugin::Status::Activated)
        {
            plugin->activate();
        }
        if(status >= IAudioPlugin::Status::Processing)
        {
            // TODO: Start/stop processing plugins on audio thread if needed
            plugin->startProcessing();
        }
        node = graphWithPDC.addNode(process);
        inserts_->insert(
            node, std::move(context), index
        );
        // TODO: Restore additional connections
        dataChanged(
            this->index(index),
            this->index(index),
            {Role::AudioInputs, Role::AudioOutputs, Role::AudioAuxInputs, Role::AudioAuxOutputs}
        );
    }
}

const YADAW::Audio::Mixer::Inserts& MixerChannelInsertListModel::inserts() const
{
    return *inserts_;
}

YADAW::Audio::Mixer::Inserts& MixerChannelInsertListModel::inserts()
{
    return *inserts_;
}

void MixerChannelInsertListModel::setFillPluginContextCallback(FillPluginContextCallback* callback)
{
    fillPluginContextCallback_ = callback;
}

void MixerChannelInsertListModel::resetFillPluginContextCallback()
{
    fillPluginContextCallback_ = &Impl::blankFillPluginContext;
}

void MixerChannelInsertListModel::updateInsertConnections(std::uint32_t from)
{
    FOR_RANGE(i, from, itemCount())
    {
        auto& context = inserts_->insertContextAt(i)->get();
        auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context.get());
        auto& pluginContextUserData = *static_cast<YADAW::Model::PluginContextUserData*>(pluginContext.userData.get());
        auto& window = pluginContext.editor;
        auto& connection = pluginContextUserData.pluginWindowConnection;
        if(window)
        {
            QObject::disconnect(connection);
            connection = QObject::connect(
                window, &QWindow::visibleChanged,
                [this, i](bool visible)
                {
                    dataChanged(this->index(i), this->index(i),
                        {Role::WindowVisible}
                    );
                }
            );
        }
        QObject::disconnect(pluginContextUserData.genericEditorWindowConnection);
        pluginContextUserData.genericEditorWindowConnection = QObject::connect(
            pluginContext.genericEditor, &QWindow::visibleChanged,
            [this, i](bool visible)
            {
                dataChanged(this->index(i), this->index(i),
                    {Role::GenericEditorVisible}
                );
            }
        );
        pluginContext.position.index = i;
    }
}
}