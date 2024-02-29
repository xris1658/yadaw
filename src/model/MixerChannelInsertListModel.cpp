#include "MixerChannelInsertListModel.hpp"

#include "audio/plugin/IAudioPlugin.hpp"
#include "audio/engine/AudioDeviceGraphProcess.hpp"
#include "audio/engine/MultiInputDeviceWithPDC.hpp"
#include "audio/util/CLAPHelper.hpp"
#include "audio/util/VestifalHelper.hpp"
#include "audio/util/VST3Helper.hpp"
#include "controller/AudioEngineController.hpp"
#include "controller/PluginListController.hpp"
#include "controller/PluginPoolController.hpp"
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
    YADAW::Audio::Mixer::Inserts& inserts, QObject* parent):
    IMixerChannelInsertListModel(parent),
    inserts_(&inserts)
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
        auto& pool = YADAW::Controller::appPluginPool();
        auto pluginInfo = YADAW::DAO::selectPluginById(pluginId);
        auto it = pool.find<QString>(pluginInfo.path);
        if(it == pool.end())
        {
            auto library = YADAW::Native::Library(pluginInfo.path);
            it = pool.emplace_hint(it, std::move(library),
                YADAW::Controller::PluginPool::value_type::second_type()
            );
        }
        std::unique_ptr<IAudioPlugin> plugin(nullptr);
        switch(pluginInfo.format)
        {
        case PluginFormat::PluginFormatVST3:
        {
            auto pluginPtr = std::make_unique<YADAW::Audio::Plugin::VST3Plugin>(
                YADAW::Audio::Util::createVST3FromLibrary(it->first)
            );
            if(pluginPtr && pluginPtr->status() != IAudioPlugin::Status::Empty)
            {
                Steinberg::TUID tuid;
                std::memcpy(tuid, pluginInfo.uid.data(), std::size(tuid));
                pluginPtr->createPlugin(tuid);
                auto vst3ComponentHandler = new(std::nothrow) YADAW::Audio::Host::VST3ComponentHandler(*pluginPtr);
                if(vst3ComponentHandler)
                {
                    pluginPtr->setComponentHandler(*vst3ComponentHandler);
                    pluginPtr->initialize(engine.sampleRate(), engine.bufferSize());
                    auto inputCount = pluginPtr->audioInputGroupCount();
                    auto outputCount = pluginPtr->audioOutputGroupCount();
                    auto device = graphWithPDC.graph().getNodeData(inserts_->inNode()).process.device();
                    auto channelGroupType = device->audioOutputGroupAt(inserts_->inChannel())->get().type();
                    std::vector<YADAW::Audio::Base::ChannelGroupType> inputChannels(inputCount,
                        channelGroupType);
                    std::vector<YADAW::Audio::Base::ChannelGroupType> outputChannels(outputCount,
                        channelGroupType);
                    pluginPtr->setChannelGroups(inputChannels.data(), inputChannels.size(), outputChannels.data(),
                        outputChannels.size());
                    pluginPtr->activate();
                    pluginPtr->startProcessing();
                    auto& vst3PluginPool = YADAW::Controller::appVST3PluginPool();
                    vst3PluginPool.emplace(
                        pluginPtr.get(),
                        YADAW::Controller::VST3PluginContext{vst3ComponentHandler}
                    );
                    auto nodeHandle = graphWithPDC.addNode(AudioDeviceProcess(*pluginPtr));
                    engine.vst3PluginPool().updateAndDispose(
                        std::make_unique<YADAW::Controller::VST3PluginPoolVector>(
                            createPoolVector(vst3PluginPool)
                        ),
                        engine.running()
                    );
                    inserts_->insert(nodeHandle, position, pluginInfo.name);
                    plugin = std::unique_ptr<IAudioPlugin>(std::move(pluginPtr));
                    ret = true;
                }
            }
            break;
        }
        case PluginFormat::PluginFormatCLAP:
        {
            auto pluginPtr = new(std::nothrow)
            YADAW::Audio::Plugin::CLAPPlugin(
                YADAW::Audio::Util::createCLAPFromLibrary(
                    it->first
                )
            );
            if(pluginPtr && pluginPtr->status() != IAudioPlugin::Status::Empty)
            {
                pluginPtr->createPlugin(pluginInfo.uid.data());
                pluginPtr->host().setMainThreadId(std::this_thread::get_id());
                pluginPtr->initialize(engine.sampleRate(), engine.bufferSize());
                pluginPtr->activate();
                pluginPtr->startProcessing();
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
                auto nodeHandle = graphWithPDC.addNode(AudioDeviceProcess(*pluginPtr));
                engine.clapPluginPool().updateAndDispose(
                    std::make_unique<YADAW::Controller::CLAPPluginPoolVector>(
                        createPoolVector(clapPluginPool)
                    ),
                    engine.running()
                );
                inserts_->insert(nodeHandle, position, pluginInfo.name);
                plugin = std::unique_ptr<IAudioPlugin>(pluginPtr);
                ret = true;
            }
            break;
        }
        // VST plugins are not loaded since the implementation of
        // `VestifalPlugin` is problematic.
        }
        poolIterators_.emplace(poolIterators_.begin() + position, it);
        if(ret)
        {
            YADAW::Controller::pluginNeedsWindow = plugin.get();
            if(plugin->gui())
            {
                YADAW::Event::eventHandler->createPluginWindow();
            }
            YADAW::Event::eventHandler->createGenericPluginEditor();
            const auto& [pluginWindow, genericEditor] = YADAW::Controller::pluginWindows;
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
            {
                auto& [window, connection] = *genericEditors_.emplace(genericEditors_.begin() + position, genericEditor);
                connection = QObject::connect(
                    window, &QWindow::visibleChanged,
                    [this, position](bool visible)
                    {
                        dataChanged(this->index(position), this->index(position), {Role::GenericEditorVisible});
                    }
                );
            }
            paramListModel_.emplace(paramListModel_.begin() + position,
                std::make_unique<YADAW::Model::PluginParameterListModel>(
                    *plugin->parameter()
                )
            );
            genericEditor->setProperty(
                "pluginName",
                QVariant::fromValue(pluginInfo.name)
            );
            genericEditor->setProperty(
                "parameterListModel",
                QVariant::fromValue<QObject*>(paramListModel_[position].get())
            );
            it->second.emplace(std::move(plugin));
            FOR_RANGE(i, position + 1, pluginEditors_.size())
            {
                auto& [window, connection] = pluginEditors_[i];
                if(window)
                {
                    QObject::disconnect(connection);
                    connection = QObject::connect(window, &QWindow::visibleChanged,
                        [this, i](bool visible)
                        {
                            dataChanged(this->index(i), this->index(i), {Role::WindowVisible});
                        }
                    );
                }
            }
            beginInsertRows(QModelIndex(), position, position);
            endInsertRows();
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
        std::set<std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>> pluginsToRemove;
        std::vector<YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>> processDataToRemove;
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        std::vector<ade::NodeHandle> removingNodes;
        std::vector<YADAW::Controller::PluginPool::iterator> removingIterators;
        std::vector<std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC>> removingMultiInputs;
        removingNodes.reserve(removeCount);
        removingIterators.reserve(removeCount);
        FOR_RANGE(i, position, position + removeCount)
        {
            removingNodes.emplace_back(*(inserts_->insertAt(i)));
            if(auto window = pluginEditors_[i].window)
            {
                window->setProperty("destroyingPlugin", QVariant::fromValue(true));
                delete window;
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
            auto libraryIt = poolIterators_[i + position];
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
                clapPlugin->stopProcessing();
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
        auto& pluginPool = YADAW::Controller::appPluginPool();
        for(auto it: removingIterators)
        {
            pluginPool.erase(it);
        }
        paramListModel_.erase(
            paramListModel_.begin() + position,
            paramListModel_.begin() + position + removeCount
        );
        pluginEditors_.erase(
            pluginEditors_.begin() + position,
            pluginEditors_.begin() + position + removeCount
        );
        genericEditors_.erase(
            genericEditors_.begin() + position,
            genericEditors_.begin() + position + removeCount
        );
        FOR_RANGE(i, position, pluginEditors_.size())
        {
            auto& [window, windowConnection] = pluginEditors_[i];
            if(window)
            {
                QObject::disconnect(windowConnection);
                windowConnection = QObject::connect(window, &QWindow::visibleChanged,
                    [this, i](bool visible)
                    {
                        dataChanged(this->index(i), this->index(i), {Role::WindowVisible});
                    }
                );
            }
        }
        FOR_RANGE(i, position, genericEditors_.size())
        {
            auto& [genericEditor, genericEditorConnection] = genericEditors_[i];
            QObject::disconnect(genericEditorConnection);
            genericEditorConnection = QObject::connect(genericEditor, &QWindow::visibleChanged,
                [this, i](bool visible)
                {
                    dataChanged(this->index(i), this->index(i), {Role::GenericEditorVisible});
                }
            );
        }
        poolIterators_.erase(
            poolIterators_.begin() + position,
            poolIterators_.begin() + position + removeCount
        );
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
}