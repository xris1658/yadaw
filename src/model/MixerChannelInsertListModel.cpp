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
            auto ret = false;
            auto plugin = dynamic_cast<const IAudioPlugin*>(device);
            if(plugin)
            {
                auto gui = plugin->gui();
                if(gui)
                {
                    auto window = gui->window();
                    if(window)
                    {
                        ret = window->isVisible();
                    }
                }
            }
            return QVariant::fromValue<bool>(ret);
        }
        case Role::GenericEditorVisible:
        {
            return QVariant::fromValue<bool>(genericEditors_[row]->isVisible());
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
            auto gui = plugin->gui();
            if(gui)
            {
                auto window = gui->window();
                if(window)
                {
                    auto visible = value.value<bool>();
                    if(window->isVisible() != visible)
                    {
                        window->setVisible(visible);
                        dataChanged(index, index, {Role::WindowVisible});
                    }
                }
            }
        }
        case Role::GenericEditorVisible:
        {
            genericEditors_[row]->setVisible(value.value<bool>());
            dataChanged(index, index, {Role::GenericEditorVisible});
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
        const auto& pluginListModel = YADAW::Controller::appPluginListModel();
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
                pluginPtr->initialize(engine.sampleRate(), engine.bufferSize());
                pluginPtr->activate();
                pluginPtr->startProcessing();
                auto nodeHandle = graphWithPDC.addNode(AudioDeviceProcess(*pluginPtr));
                inserts_->insert(nodeHandle, position, pluginInfo.name);
                plugin = std::unique_ptr<IAudioPlugin>(std::move(pluginPtr));
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
            auto& audioEngine = YADAW::Controller::AudioEngine::appAudioEngine();
            YADAW::Controller::pluginNeedsWindow = plugin.get();
            if(plugin->gui())
            {
                YADAW::Event::eventHandler->createPluginWindow();
            }
            YADAW::Event::eventHandler->createGenericPluginEditor();
            auto& appPluginWindowPool = YADAW::Controller::appPluginWindowPool();
            if(auto it = appPluginWindowPool.find(plugin.get());
                it != appPluginWindowPool.end())
            {
                auto [pluginWindow, genericEditor] = it->second;
                if(pluginWindow)
                {
                    pluginWindow->setTitle(pluginInfo.name);
                    pluginWindow->show();
                }
                else
                {
                    genericEditor->show();
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
                genericEditor->setTitle(pluginInfo.name);
                genericEditors_.emplace(genericEditors_.begin() + position,
                    genericEditor
                );
            }
            it->second.emplace(std::move(plugin));
            beginInsertRows(QModelIndex(), position, position);
            endInsertRows();
            auto& graph = audioEngine.mixer().graph().graph();
            auto& bufferExt = audioEngine.mixer().bufferExtension();
            auto& processSequence = audioEngine.processSequence();
            processSequence.updateAndDispose(
                std::make_unique<YADAW::Audio::Engine::ProcessSequence>(
                    YADAW::Audio::Engine::getProcessSequence(graph, bufferExt)
                ),
                audioEngine.running()
            );

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
        auto& pluginWindowPool = YADAW::Controller::appPluginWindowPool();
        std::vector<ade::NodeHandle> removingNodes;
        std::vector<YADAW::Controller::PluginPool::iterator> removingIterators;
        removingNodes.reserve(removeCount);
        removingIterators.reserve(removeCount);
        FOR_RANGE(i, position, position + removeCount)
        {
            removingNodes.emplace_back(*(inserts_->insertAt(i)));
        }
        inserts_->remove(position, removeCount);
        for(const auto& removingNode: removingNodes)
        {
            processDataToRemove.emplace_back(std::move(bufferExt.getData(removingNode).container));
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
            if(auto pluginWindowIt = pluginWindowPool.find(pluginToRemove);
                pluginWindowIt != pluginWindowPool.end())
            {
                auto& [pluginWindow, genericEditor] = pluginWindowIt->second;
                if(pluginWindow)
                {
                    pluginWindow->setProperty("destroyingPlugin", QVariant::fromValue(true));
                    pluginToRemove->gui()->detachWithWindow();
                    delete pluginWindow;
                }
                if(genericEditor)
                {
                    genericEditor->setProperty("destroyingPlugin", QVariant::fromValue(true));
                    delete genericEditor;
                }
                pluginWindowPool.erase(pluginWindowIt);
            }
            auto it = plugins.find(pluginToRemove);
            if(it != plugins.end())
            {
                pluginsToRemove.insert(plugins.extract(it));
            }
            if(plugins.empty())
            {
                removingIterators.emplace_back(libraryIt);
            }
            graphWithPDC.removeNode(removingNodes[i]);
        }
        auto& processSequence = audioEngine.processSequence();
        processSequence.updateAndDispose(
            std::make_unique<YADAW::Audio::Engine::ProcessSequence>(
                YADAW::Audio::Engine::getProcessSequence(graph, bufferExt)
            ),
            audioEngine.running()
        );
        for(auto& pluginToRemove: pluginsToRemove)
        {
            switch(pluginToRemove->format())
            {
            case IAudioPlugin::Format::VST3:
            {
                auto vst3Plugin = static_cast<YADAW::Audio::Plugin::VST3Plugin*>(pluginToRemove.get());
                auto& vst3PluginPool = YADAW::Controller::appVST3PluginPool();
                vst3PluginPool.erase(
                    vst3PluginPool.find(vst3Plugin)
                );
                YADAW::Controller::AudioEngine::appAudioEngine().vst3PluginPool().updateAndDispose(
                    std::make_unique<YADAW::Controller::VST3PluginPoolVector>(
                        createPoolVector(vst3PluginPool)
                    ),
                    audioEngine.running()
                );
                vst3Plugin->stopProcessing();
                vst3Plugin->deactivate();
                vst3Plugin->uninitialize();
                delete vst3Plugin->componentHandler();
                break;
            }
            case IAudioPlugin::Format::CLAP:
            {
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
        genericEditors_.erase(
            genericEditors_.begin() + position,
            genericEditors_.begin() + position + removeCount
        );
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