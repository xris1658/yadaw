#include "PluginLatencyUpdatedCallback.hpp"

#include "controller/AudioEngineController.hpp"
#include "model/MixerChannelInsertListModel.hpp"

namespace YADAW::Controller
{
namespace Impl
{
void latencyUpdated(Audio::Plugin::IAudioPlugin& plugin)
{
    auto& pluginContextMap = YADAW::Controller::appPluginContextMap();
    if(auto it = pluginContextMap.find(&plugin); it != pluginContextMap.end())
    {
        auto& context = it->second;
        // Inform the graph
        auto nodeHandle = *(context.insertListModel->inserts().insertAt(context.insertIndex));
        auto& upstreamLatencyExt = YADAW::Controller::AudioEngine::appAudioEngine().mixer().graph().upstreamLatencyExtension();
        upstreamLatencyExt.onLatencyOfNodeUpdated(nodeHandle);
        // Inform the model
        context.insertListModel->latencyUpdated(context.insertIndex);
    }
}
}
}