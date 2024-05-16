#include "PluginIOConfigUpdatedCallback.hpp"

#include "controller/AudioEngineController.hpp"
#include "model/MixerChannelInsertListModel.hpp"
#include "model/MixerChannelListModel.hpp"

namespace YADAW::Controller
{
void Impl::ioConfigUpdated(YADAW::Audio::Plugin::IAudioPlugin& plugin)
{
    auto& pluginContextMap = YADAW::Controller::appPluginContextMap();
    if(auto it = pluginContextMap.find(&plugin); it != pluginContextMap.end())
    {
        auto& context = it->second;
        if(context.position == YADAW::Controller::PluginContext::Position::Insert)
        {
            auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(context.model);
            insertListModel->updateIOConfig(context.index);
        }
        else if(context.position == YADAW::Controller::PluginContext::Position::Instrument)
        {
            auto channelListModel = static_cast<YADAW::Model::MixerChannelListModel*>(context.model);
            channelListModel->updateInstrumentIOConfig(context.index);
        }
    }
}
}
