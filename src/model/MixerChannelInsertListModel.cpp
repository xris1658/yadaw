#include "MixerChannelInsertListModel.hpp"

#include "audio/plugin/IAudioPlugin.hpp"
#include "audio/engine/AudioDeviceGraphProcess.hpp"
#include "audio/engine/MultiInputDeviceWithPDC.hpp"
#include "audio/util/CLAPHelper.hpp"
#include "audio/util/VestifalHelper.hpp"
#include "audio/util/VST3Helper.hpp"
#include "controller/AudioEngineController.hpp"
#include "controller/LibraryPluginMap.hpp"
#include "controller/PluginContextMap.hpp"
#include "controller/PluginController.hpp"
#include "controller/PluginLatencyUpdatedCallback.hpp"
#include "controller/PluginListController.hpp"
#include "controller/PluginWindowController.hpp"
#include "controller/VST3PluginPool.hpp"
#include "event/EventBase.hpp"
#include "dao/PluginTable.hpp"

#include <new>

namespace YADAW::Model
{
using YADAW::Audio::Engine::AudioDeviceProcess;
using YADAW::Audio::Engine::MultiInputDeviceWithPDC;
using YADAW::Audio::Plugin::IAudioPlugin;

MixerChannelInsertListModel::MixerChannelInsertListModel(
    YADAW::Audio::Mixer::Inserts& inserts,
    YADAW::Model::MixerChannelListModel::ListType type,
    std::uint32_t channelIndex,
    bool isPreFaderInsert,
    std::uint32_t insertsIndex,
    QObject* parent):
    IMixerChannelInsertListModel(parent),
    inserts_(&inserts),
    type_(type),
    channelIndex_(channelIndex),
    isPreFaderInsert_(isPreFaderInsert),
    insertsIndex_(insertsIndex)
{}

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
        const auto& node = *(inserts_->insertAt(row));
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
        switch(role)
        {
        case Role::Name:
        {
            return QVariant::fromValue<QString>(*inserts_->insertNameAt(row));
        }
        case Role::Bypassed:
        {
            return QVariant::fromValue<bool>(*inserts_->insertBypassed(row));
        }
        case Role::AudioInputs:
        {
            return QVariant::fromValue<QObject*>(inputAudioChannelGroupLists_[row].get());
        }
        case Role::AudioOutputs:
        {
            return QVariant::fromValue<QObject*>(outputAudioChannelGroupLists_[row].get());
        }
        case Role::HasUI:
        {
            auto ret = false;
            auto plugin = dynamic_cast<const IAudioPlugin*>(device);
            if(plugin)
            {
                auto gui = plugin->gui();
                ret = gui;
            }
            return QVariant::fromValue<bool>(ret);
        }
        case Role::WindowVisible:
        {
            const auto& pluginEditor = pluginEditors_[row];
            return QVariant::fromValue<bool>(pluginEditor.window && pluginEditor.window->isVisible());
        }
        case Role::GenericEditorVisible:
        {
            const auto& genericEditor = genericEditors_[row];
            return QVariant::fromValue<bool>(genericEditor.window && genericEditor.window->isVisible());
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
        auto node = *(inserts_->insertAt(row));
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
        auto plugin = static_cast<YADAW::Audio::Plugin::IAudioPlugin*>(device);
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
            if(auto window = pluginEditors_[row].window)
            {
                window->setVisible(value.value<bool>());
                return true;
            }
            return false;
        }
        case Role::GenericEditorVisible:
        {
            genericEditors_[row].window->setVisible(value.value<bool>());
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
        using PluginFormat = YADAW::DAO::PluginFormat;
        auto& engine = YADAW::Controller::AudioEngine::appAudioEngine();
        auto& graphWithPDC = engine.mixer().graph();
        auto& libraryPluginMap = YADAW::Controller::appLibraryPluginMap();
        auto pluginInfo = YADAW::DAO::selectPluginById(pluginId);
        auto optionalCreatePluginResult = YADAW::Controller::createPlugin(
            pluginInfo.path, static_cast<YADAW::DAO::PluginFormat>(pluginInfo.format),
            pluginInfo.uid
        );
        if(!optionalCreatePluginResult.has_value())
        {
            return false;
        }
        auto& [it, pluginAndProcess] = *optionalCreatePluginResult;
        auto& [plugin, process] = pluginAndProcess;
        switch(pluginInfo.format)
        {
        case PluginFormat::PluginFormatVST3:
        {
            auto pluginPtr = static_cast<YADAW::Audio::Plugin::VST3Plugin*>(plugin);
            auto vst3ComponentHandler = new(std::nothrow) YADAW::Audio::Host::VST3ComponentHandler(*pluginPtr);
            if(vst3ComponentHandler)
            {
                vst3ComponentHandler->setLatencyChangedCallback(
                    &YADAW::Controller::latencyUpdated<YADAW::Audio::Plugin::VST3Plugin>
                );
                pluginPtr->initialize(engine.sampleRate(), engine.bufferSize());
                auto inputCount = pluginPtr->audioInputGroupCount();
                auto outputCount = pluginPtr->audioOutputGroupCount();
                auto device = graphWithPDC.graph().getNodeData(inserts_->inNode()).process.device();
                auto channelGroupType = device->audioOutputGroupAt(inserts_->inChannelGroupIndex())->get().type();
                std::vector<YADAW::Audio::Base::ChannelGroupType> inputChannels(inputCount,
                    channelGroupType);
                std::vector<YADAW::Audio::Base::ChannelGroupType> outputChannels(outputCount,
                    channelGroupType);
                pluginPtr->setChannelGroups(
                    inputChannels.data(), inputChannels.size(),
                    outputChannels.data(), outputChannels.size()
                );
                pluginPtr->activate();
                pluginPtr->startProcessing();
                auto& vst3PluginPool = YADAW::Controller::appVST3PluginPool();
                vst3PluginPool.emplace(
                    pluginPtr,
                    YADAW::Controller::VST3PluginContext{vst3ComponentHandler}
                );
                auto nodeHandle = graphWithPDC.addNode(std::move(process));
                engine.vst3PluginPool().updateAndDispose(
                    std::make_unique<YADAW::Controller::VST3PluginPoolVector>(
                        createPoolVector(vst3PluginPool)
                    ),
                    engine.running()
                );
                inserts_->insert(nodeHandle, position, pluginInfo.name);
                ret = true;
            }
            break;
        }
        case PluginFormat::PluginFormatCLAP:
        {
            auto pluginPtr = static_cast<YADAW::Audio::Plugin::CLAPPlugin*>(plugin);
            pluginPtr->host().setLatencyChangedCallback(
                &YADAW::Controller::latencyUpdated<YADAW::Audio::Plugin::CLAPPlugin>
            );
            YADAW::Audio::Host::CLAPHost::setMainThreadId(std::this_thread::get_id());
            pluginPtr->initialize(engine.sampleRate(), engine.bufferSize());
            pluginPtr->activate();
            auto clapEventList = new(std::nothrow) YADAW::Audio::Host::CLAPEventList();
            if(clapEventList)
            {
                clapEventList->attachToProcessData(pluginPtr->processData());
            }
            auto& clapPluginPool = YADAW::Controller::appCLAPPluginPool();
            clapPluginPool.emplace(
                pluginPtr,
                YADAW::Controller::CLAPPluginContext{clapEventList}
            );
            auto nodeHandle = graphWithPDC.addNode(std::move(process));
            engine.clapPluginToSetProcess().update(
                std::make_unique<YADAW::Controller::CLAPPluginToSetProcessVector>(
                    1, std::make_pair(pluginPtr, true)
                ),
                engine.running()
            );
            engine.clapPluginPool().update(
                std::make_unique<YADAW::Controller::CLAPPluginPoolVector>(
                    createPoolVector(clapPluginPool)
                ),
                engine.running()
            );
            engine.clapPluginPool().disposeOld(engine.running());
            engine.clapPluginToSetProcess().disposeOld(engine.running());
            inserts_->insert(nodeHandle, position, pluginInfo.name);
            ret = true;
            break;
        }
        // VST plugins are not loaded since the implementation of
        // `VestifalPlugin` is problematic.
        case PluginFormat::PluginFormatVestifal:
        {
            ret = false;
            break;
        }
        }
        if(ret)
        {
            inputAudioChannelGroupLists_.emplace(
                inputAudioChannelGroupLists_.begin() + position,
                std::make_unique<YADAW::Model::AudioDeviceIOGroupListModel>(
                    *plugin, true
                )
            );
            outputAudioChannelGroupLists_.emplace(
                outputAudioChannelGroupLists_.begin() + position,
                std::make_unique<YADAW::Model::AudioDeviceIOGroupListModel>(
                    *plugin, false
                )
            );
            libraryPluginIterators_.emplace(
                libraryPluginIterators_.begin() + position,
                it
            );
            auto& pluginContextMap = YADAW::Controller::appPluginContextMap();
            const auto& [pluginContextIterator, inserted] = pluginContextMap.emplace(
                plugin,
                YADAW::Controller::PluginContext()
            );
            assert(inserted);
            auto& context = pluginContextIterator->second;
            context.position = YADAW::Controller::PluginContext::Position::Insert;
            context.model = this;
            context.index = position;
            pluginContextIterators_.emplace(
                pluginContextIterators_.begin() + position,
                pluginContextIterator
            );
            YADAW::Controller::pluginNeedsWindow = plugin;
            if(plugin->gui())
            {
                YADAW::Controller::createPluginWindow();
            }
            YADAW::Controller::createGenericPluginEditor();
            auto& [pluginWindow, genericEditor] = YADAW::Controller::pluginWindows;
            if(pluginWindow)
            {
                pluginWindow->setTitle(pluginInfo.name);
                auto& [window, connection] = *pluginEditors_.emplace(pluginEditors_.begin() + position, pluginWindow);
                pluginWindow->show();
                connection = QObject::connect(
                    window, &QWindow::visibleChanged,
                    [this, position](bool visible)
                    {
                        dataChanged(this->index(position), this->index(position), {Role::WindowVisible});
                    }
                );
            }
            else
            {
                pluginEditors_.emplace(pluginEditors_.begin() + position, nullptr);
                genericEditor->show();
            }
            auto& genericEditorConnection = genericEditors_.emplace(
                genericEditors_.begin() + position, genericEditor
            )->connection;
            genericEditorConnection = QObject::connect(
                genericEditor, &QWindow::visibleChanged,
                [this, position](bool visible)
                {
                    dataChanged(this->index(position), this->index(position), {Role::GenericEditorVisible});
                }
            );
            paramListModel_.emplace(paramListModel_.begin() + position,
                std::make_unique<YADAW::Model::PluginParameterListModel>(
                    *plugin->parameter()
                )
            );
            genericEditor->setTitle(pluginInfo.name);
            genericEditor->setProperty(
                "parameterListModel",
                QVariant::fromValue<QObject*>(paramListModel_[position].get())
            );
            pluginWindow = nullptr;
            genericEditor = nullptr;
            updateInsertConnections(position + 1);
            beginInsertRows(QModelIndex(), position, position);
            endInsertRows();
        }
        else
        {
            auto& pluginPool = it->second;
            auto pluginIt = pluginPool.find(plugin);
            if(pluginIt != pluginPool.end())
            {
                pluginPool.erase(pluginIt);
                if(pluginPool.empty())
                {
                    libraryPluginMap.erase(it);
                }
            }
        }
    }
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
        && position + removeCount >= 0 && position + removeCount <= inserts_->insertCount())
    {
        ret = true;
        auto& audioEngine = YADAW::Controller::AudioEngine::appAudioEngine();
        auto& graphWithPDC = audioEngine.mixer().graph();
        auto& graph = audioEngine.mixer().graph().graph();
        auto& bufferExt = audioEngine.mixer().bufferExtension();
        auto& pluginContextMap = YADAW::Controller::appPluginContextMap();
        std::set<std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>> pluginsToRemove;
        std::vector<YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>> processDataToRemove;
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        std::vector<ade::NodeHandle> removingNodes;
        std::vector<YADAW::Controller::LibraryPluginMap::iterator> removingIterators;
        std::vector<std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC>> removingMultiInputs;
        removingNodes.reserve(removeCount);
        removingIterators.reserve(removeCount);
        FOR_RANGE(i, position, position + removeCount)
        {
            removingNodes.emplace_back(*(inserts_->insertAt(i)));
            if(auto window = pluginEditors_[i].window)
            {
                window->setProperty("destroyingPlugin", QVariant::fromValue(true));
            }
            auto genericEditor = genericEditors_[i].window;
            genericEditor->setProperty("destroyingPlugin", QVariant::fromValue(true));
            delete genericEditor;
        }
        inserts_->remove(position, removeCount);
        for(const auto& removingNode: removingNodes)
        {
            processDataToRemove.emplace_back(bufferExt.getData(removingNode).container);
        }
        endRemoveRows();
        FOR_RANGE0(i, removingNodes.size())
        {
            auto device = graph.getNodeData(removingNodes[i]).process.device();
            YADAW::Audio::Plugin::IAudioPlugin* pluginToRemove = nullptr;
            if(device->audioInputGroupCount() > 1)
            {
                // `static_cast` is okay since we always call
                // `AudioDeviceGraphWithPDC::addNode` to add devices (those with
                // multiple inputs also included) into the graph.
                auto multiInput = static_cast<MultiInputDeviceWithPDC*>(device);
                // `static_cast` is okay since we only add
                // `IAudioPlugin`s into the inserts.
                pluginToRemove = static_cast<IAudioPlugin*>(
                    multiInput->process().device()
                );
            }
            else
            {
                // `static_cast` is okay since we only add
                // `IAudioPlugin`s into the inserts.
                pluginToRemove = static_cast<IAudioPlugin*>(device);
            }
            auto libraryIt = libraryPluginIterators_[i + position];
            auto& [library, plugins] = *libraryIt;
            auto it = plugins.find(pluginToRemove);
            if(it != plugins.end())
            {
                pluginsToRemove.insert(plugins.extract(it));
            }
            if(plugins.empty())
            {
                removingIterators.emplace_back(libraryIt);
            }
            auto removingMultiInput = graphWithPDC.removeNode(removingNodes[i]);
            if(removingMultiInput)
            {
                removingMultiInputs.emplace_back(std::move(removingMultiInput));
            }
        }
        audioEngine.insertsNodeRemovedCallback(*inserts_);
        removingMultiInputs.clear();
        for(auto& pluginToRemove: pluginsToRemove)
        {
            auto pluginPtr = pluginToRemove.get();
            auto pluginGUI = pluginPtr->gui();
            if(pluginGUI)
            {
                pluginGUI->detachWithWindow();
            }
            switch(pluginToRemove->format())
            {
            case IAudioPlugin::Format::VST3:
            {
                auto vst3Plugin = static_cast<YADAW::Audio::Plugin::VST3Plugin*>(pluginToRemove.get());
                auto& vst3PluginPool = YADAW::Controller::appVST3PluginPool();
                auto it = vst3PluginPool.find(vst3Plugin);
                auto componentHandler = it->second.componentHandler;
                vst3PluginPool.erase(it);
                audioEngine.vst3PluginPool().updateAndDispose(
                    std::make_unique<YADAW::Controller::VST3PluginPoolVector>(
                        createPoolVector(vst3PluginPool)
                    ),
                    audioEngine.running()
                );
                vst3Plugin->stopProcessing();
                vst3Plugin->deactivate();
                vst3Plugin->uninitialize();
                delete componentHandler;
                break;
            }
            case IAudioPlugin::Format::CLAP:
            {
                auto clapPlugin = static_cast<YADAW::Audio::Plugin::CLAPPlugin*>(pluginToRemove.get());
                auto& clapPluginPool = YADAW::Controller::appCLAPPluginPool();
                auto it = clapPluginPool.find(clapPlugin);
                auto eventList = it->second.eventList;
                clapPluginPool.erase(it);
                audioEngine.clapPluginPool().updateAndDispose(
                    std::make_unique<YADAW::Controller::CLAPPluginPoolVector>(
                        createPoolVector(clapPluginPool)
                    ),
                    audioEngine.running()
                );
                audioEngine.clapPluginToSetProcess().update(
                    std::make_unique<YADAW::Controller::CLAPPluginToSetProcessVector>(
                        1, std::make_pair(clapPlugin, false)
                    ),
                    audioEngine.running()
                );
                // Completion of the `update` above does NOT mean that the
                // plugin has stopped processing. It is indicated by completion
                // of the following `updateAndDispose` because the last audio
                // callback is finished by then.
                audioEngine.clapPluginToSetProcess().updateAndDispose(
                    std::make_unique<YADAW::Controller::CLAPPluginToSetProcessVector>(),
                    audioEngine.running()
                );
                clapPlugin->deactivate();
                clapPlugin->uninitialize();
                delete eventList;
                break;
            }
            case IAudioPlugin::Format::Vestifal:
            {
                break;
            }
            }
        }
        pluginsToRemove.clear();
        processDataToRemove.clear();
        auto& pluginPool = YADAW::Controller::appLibraryPluginMap();
        for(auto it: removingIterators)
        {
            pluginPool.erase(it);
        }
        paramListModel_.erase(
            paramListModel_.begin() + position,
            paramListModel_.begin() + position + removeCount
        );
        inputAudioChannelGroupLists_.erase(
            inputAudioChannelGroupLists_.begin() + position,
            inputAudioChannelGroupLists_.begin() + position + removeCount
        );
        outputAudioChannelGroupLists_.erase(
            outputAudioChannelGroupLists_.begin() + position,
            outputAudioChannelGroupLists_.begin() + position + removeCount
        );
        FOR_RANGE(i, position, position + removeCount)
        {
            QObject::disconnect(pluginEditors_[i].connection);
            QObject::disconnect(genericEditors_[i].connection);
        }
        FOR_RANGE(i, position, position + removeCount)
        {
            auto window = pluginEditors_[i].window;
            if(window)
            {
                window->close();
                delete window;
            }
        }
        pluginEditors_.erase(
            pluginEditors_.begin() + position,
            pluginEditors_.begin() + position + removeCount
        );
        genericEditors_.erase(
            genericEditors_.begin() + position,
            genericEditors_.begin() + position + removeCount
        );
        libraryPluginIterators_.erase(
            libraryPluginIterators_.begin() + position,
            libraryPluginIterators_.begin() + position + removeCount
        );
        FOR_RANGE(i, position, position + removeCount)
        {
            pluginContextMap.erase(pluginContextIterators_[i]);
        }
        pluginContextIterators_.erase(
            pluginContextIterators_.begin() + position,
            pluginContextIterators_.begin() + position + removeCount
        );
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

void MixerChannelInsertListModel::setChannelIndex(std::uint32_t channelIndex)
{
    channelIndex_ = channelIndex;
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

const YADAW::Audio::Mixer::Inserts& MixerChannelInsertListModel::inserts() const
{
    return *inserts_;
}

YADAW::Audio::Mixer::Inserts& MixerChannelInsertListModel::inserts()
{
    return *inserts_;
}

void MixerChannelInsertListModel::updateInsertConnections(std::uint32_t from)
{
    FOR_RANGE(i, from, pluginEditors_.size())
    {
        auto& [window, connection] = pluginEditors_[i];
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
        QObject::disconnect(genericEditors_[i].connection);
        genericEditors_[i].connection = QObject::connect(
            genericEditors_[i].window, &QWindow::visibleChanged,
            [this, i](bool visible)
            {
                dataChanged(this->index(i), this->index(i),
                    {Role::GenericEditorVisible}
                );
            }
        );
    }
    FOR_RANGE(i, from, pluginContextIterators_.size())
    {
        pluginContextIterators_[i]->second.index = i;
    }
}
}