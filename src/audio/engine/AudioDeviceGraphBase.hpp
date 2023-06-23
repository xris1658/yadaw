#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE

#include "audio/engine/AudioDeviceProcess.hpp"
#include "audio/util/SampleDelay.hpp"

#include <ade/node.hpp>
#include <ade/edge.hpp>
#include <ade/graph.hpp>
#include <ade/typed_graph.hpp>
#include <ade/typed_metadata.hpp>

#include <memory>
#include <set>

namespace YADAW::Audio::Engine
{
class AudioDeviceGraphBase
{
protected:
    struct AudioDeviceProcessNode
    {
        AudioDeviceProcess process;
        AudioProcessData<float> processData;
        std::uint32_t upstreamLatency = 0U;
        std::uint32_t sumLatency() const
        {
            return process.device()->latencyInSamples() + upstreamLatency;
        }
        static const char* name() { return "AudioDeviceProcessNode"; }
    };
    struct ChannelEdge
    {
        std::uint32_t fromChannelIndex;
        std::uint32_t toChannelIndex;
        static const char* name() { return "ChannelEdge"; }
    };
public:
    AudioDeviceGraphBase();
    ade::NodeHandle addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData);
    void removeNode(ade::NodeHandle nodeHandle);
    ade::EdgeHandle connect(ade::NodeHandle from, ade::NodeHandle to, std::uint32_t fromChannel, std::uint32_t toChannel);
    void disconnect(ade::EdgeHandle edgeHandle);
private:
    ade::Graph graph_;
    ade::TypedGraph<AudioDeviceProcessNode, ChannelEdge> typedGraph_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE
