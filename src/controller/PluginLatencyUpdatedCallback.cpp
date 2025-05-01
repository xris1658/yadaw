#include "PluginLatencyUpdatedCallback.hpp"

#include "controller/AudioEngineController.hpp"
#include "model/MixerChannelInsertListModel.hpp"
#include "model/MixerChannelListModel.hpp"

namespace YADAW::Controller
{
namespace Impl
{
void latencyUpdated(Audio::Plugin::IAudioPlugin& plugin)
{
    auto& pluginContextMap = YADAW::Controller::appPluginPosition();
    if(auto it = pluginContextMap.find(&plugin); it != pluginContextMap.end())
    {
        auto& context = it->second;
        if(context.position == YADAW::Controller::PluginPosition::InChannelPosition::Insert)
        {
            auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(context.model);
            // Inform the graph
            auto nodeHandle = *(insertListModel->inserts().insertNodeAt(context.index));
            auto& upstreamLatencyExt = YADAW::Controller::AudioEngine::appAudioEngine().mixer().graph().upstreamLatencyExtension();
            upstreamLatencyExt.onLatencyOfNodeUpdated(nodeHandle);
            // Inform the model
            insertListModel->latencyUpdated(context.index);
        }
        else if(context.position == YADAW::Controller::PluginPosition::InChannelPosition::Instrument)
        {
            auto channelListModel = static_cast<YADAW::Model::MixerChannelListModel*>(context.model);
            channelListModel->instrumentLatencyUpdated(context.index);
        }
    }
}
}
}