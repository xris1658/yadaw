#include "MixerChannelInsertListModel.hpp"

#include "audio/plugin/IAudioPlugin.hpp"
#include "audio/engine/MultiInputDeviceWithPDC.hpp"
#include "audio/util/CLAPHelper.hpp"
#include "audio/util/VestifalHelper.hpp"
#include "audio/util/VST3Helper.hpp"
#include "controller/AudioEngineController.hpp"
#include "controller/PluginListController.hpp"
#include "controller/PluginPoolController.hpp"
#include "dao/PluginTable.hpp"

#include <new>

namespace YADAW::Model
{
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
            auto plugin = dynamic_cast<IAudioPlugin*>(device);
            if(plugin)
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
        auto& graph = engine.mixer().graph();
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
        std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin> plugin(nullptr);
        switch(pluginInfo.format)
        {
        case PluginFormat::PluginFormatVST3:
        {
            auto pluginPtr = std::make_unique<YADAW::Audio::Plugin::VST3Plugin>(
                std::move(YADAW::Audio::Util::createVST3FromLibrary(it->first))
            );
            if(pluginPtr && pluginPtr->status() != IAudioPlugin::Status::Empty)
            {
                Steinberg::TUID tuid;
                std::memcpy(tuid, pluginInfo.uid.data(), std::size(tuid));
                pluginPtr->createPlugin(tuid);
                pluginPtr->initialize(engine.sampleRate(), engine.bufferSize());
                auto inputCount = pluginPtr->audioInputGroupCount();
                auto outputCount = pluginPtr->audioOutputGroupCount();
                std::vector<YADAW::Audio::Base::ChannelGroupType> inputChannels(inputCount,
                    YADAW::Audio::Base::ChannelGroupType::eStereo); // FIXME: Pass channel group type here
                std::vector<YADAW::Audio::Base::ChannelGroupType> outputChannels(outputCount,
                    YADAW::Audio::Base::ChannelGroupType::eStereo);
                pluginPtr->setChannelGroups(inputChannels.data(), inputChannels.size(), outputChannels.data(),
                    outputChannels.size());
                pluginPtr->activate();
                pluginPtr->startProcessing();
                auto nodeHandle = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*pluginPtr));
                inserts_->insert(nodeHandle, position, pluginInfo.name);
                plugin = std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>(std::move(pluginPtr));
                it->second.emplace(std::move(plugin));
                ret = true;
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
                auto nodeHandle = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*pluginPtr));
                inserts_->insert(nodeHandle, position, pluginInfo.name);
                plugin = std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>(std::move(pluginPtr));
                it->second.emplace(std::move(plugin));
                ret = true;
            }
            break;
        }
        case PluginFormat::PluginFormatVestifal:
        {
            std::uint32_t uid;
            std::memcpy(&uid, pluginInfo.uid.data(), sizeof(uid));
            auto pluginPtr = new(std::nothrow) YADAW::Audio::Plugin::VestifalPlugin(
                YADAW::Audio::Util::createVestifalFromLibrary(
                    it->first, uid
                )
            );
            if(pluginPtr && pluginPtr->status() != IAudioPlugin::Status::Empty)
            {
                pluginPtr->initialize(engine.sampleRate(), engine.bufferSize());
                pluginPtr->activate();
                pluginPtr->startProcessing();
                auto nodeHandle = graph.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*pluginPtr));
                inserts_->insert(nodeHandle, position, pluginInfo.name);
                plugin = std::unique_ptr<YADAW::Audio::Plugin::IAudioPlugin>(std::move(pluginPtr));
                it->second.emplace(std::move(plugin));
                ret = true;
            }
            break;
        }
        }
        poolIterators_.emplace(poolIterators_.begin() + position, it);
    }
    if(ret)
    {
        beginInsertRows(QModelIndex(), position, position);
        endInsertRows();
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
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        std::vector<ade::NodeHandle> removingNodes;
        std::vector<YADAW::Controller::PluginPool::iterator> removingIterators;
        removingNodes.reserve(removeCount);
        removingIterators.reserve(removeCount);
        FOR_RANGE(i, position, position + removeCount)
        {
            removingNodes.emplace_back(*(inserts_->insertAt(i)));
        }
        inserts_->remove(position, removeCount);
        endRemoveRows();
        auto& audioEngine = YADAW::Controller::AudioEngine::appAudioEngine();
        auto& graphWithPDC = audioEngine.mixer().graph();
        auto& graph = graphWithPDC.graph();
        FOR_RANGE0(i, removingNodes.size())
        {
            // `static_cast` is okay since we only add
            // `IAudioPlugin`s into the inserts.
            auto device = graph.getNodeData(removingNodes[i]).process.device();
            auto plugin = static_cast<YADAW::Audio::Plugin::IAudioPlugin*>(
                device
            );
            graphWithPDC.removeNode(removingNodes[i]);
            auto& plugins = poolIterators_[i + position]->second;
            auto it = plugins.find(plugin);
            if(it != plugins.end())
            {
                plugins.erase(it);
            }
            if(plugins.empty())
            {
                removingIterators.emplace_back(poolIterators_[i + position]);
            }
        }
        auto& pluginPool = YADAW::Controller::appPluginPool();
        // for(auto it: removingIterators)
        // {
        //     pluginPool.erase(it);
        // }
    }
    return ret;
}

bool MixerChannelInsertListModel::replace(int pluginId)
{
    return false;
}

void MixerChannelInsertListModel::clear()
{
    remove(0, inserts_->insertCount());
}
}