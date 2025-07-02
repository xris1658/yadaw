#include "PluginLatencyUpdatedCallback.hpp"

#include "controller/AudioEngineController.hpp"
#include "controller/PluginContext.hpp"
#include "model/MixerChannelInsertListModel.hpp"
#include "model/MixerChannelListModel.hpp"

namespace YADAW::Controller
{
namespace Impl
{
void latencyUpdated(Audio::Plugin::IAudioPlugin& plugin)
{
    auto& pluginContexts = YADAW::Controller::appPluginContexts();
    if(auto it = pluginContexts.find(&plugin); it != pluginContexts.end())
    {
        auto& context = it->position;
        if(context.position == YADAW::Controller::PluginPosition::InChannelPosition::Insert)
        {
            auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(context.model);
            // Inform the graph
            auto& upstreamLatencyExt = YADAW::Controller::AudioEngine::appAudioEngine().mixer().graph().upstreamLatencyExtension();
            upstreamLatencyExt.onLatencyOfNodeUpdated(it->node);
            // Inform the model
            insertListModel->latencyUpdated(context.index);
        }
        else if(context.position == YADAW::Controller::PluginPosition::InChannelPosition::Instrument)
        {
            auto channelListModel = static_cast<YADAW::Model::MixerChannelListModel*>(context.model);
            // Inform the graph
            auto& upstreamLatencyExt = channelListModel->mixer().graph().upstreamLatencyExtension();
            upstreamLatencyExt.onLatencyOfNodeUpdated(it->node);
            // Inform the model
            channelListModel->instrumentLatencyUpdated(context.index);
        }
    }
}
}
}