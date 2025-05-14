#include "PluginIOConfigUpdatedCallback.hpp"

#include "controller/AudioEngineController.hpp"
#include "controller/PluginContext.hpp"
#include "model/MixerChannelInsertListModel.hpp"
#include "model/MixerChannelListModel.hpp"

namespace YADAW::Controller
{
void Impl::ioConfigUpdated(YADAW::Audio::Plugin::IAudioPlugin& plugin)
{
    auto& pluginContextMap = YADAW::Controller::appPluginPosition();
    if(auto it = pluginContextMap.find(&plugin); it != pluginContextMap.end())
    {
        auto& context = it->second;
        if(context.position == YADAW::Controller::PluginPosition::InChannelPosition::Insert)
        {
            auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(context.model);
            insertListModel->updateIOConfig(context.index);
        }
        else if(context.position == YADAW::Controller::PluginPosition::InChannelPosition::Instrument)
        {
            auto channelListModel = static_cast<YADAW::Model::MixerChannelListModel*>(context.model);
            channelListModel->updateInstrumentIOConfig(context.index);
        }
    }
    auto& pluginContexts = YADAW::Controller::appPluginContexts();
    if(auto it = pluginContexts.find(&plugin); it != pluginContexts.end())
    {
        auto& context = it->position;
        if(context.position == YADAW::Controller::PluginPosition::InChannelPosition::Insert)
        {
            auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(context.model);
            insertListModel->updateIOConfig(context.index);
        }
        else if(context.position == YADAW::Controller::PluginPosition::InChannelPosition::Instrument)
        {
            auto channelListModel = static_cast<YADAW::Model::MixerChannelListModel*>(context.model);
            channelListModel->updateInstrumentIOConfig(context.index);
        }
    }
}
}
