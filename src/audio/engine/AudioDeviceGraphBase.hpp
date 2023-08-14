#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE

#include "audio/engine/AudioDeviceProcess.hpp"
#include "audio/util/SampleDelay.hpp"
#include "util/ADEUtil.hpp"

#include <ade/node.hpp>
#include <ade/edge.hpp>
#include <ade/graph.hpp>
#include <ade/typed_graph.hpp>
#include <ade/typed_metadata.hpp>

#include <set>
#include <unordered_map>

namespace YADAW::Audio::Engine
{
class AudioDeviceGraphBase
{
public:
    struct AudioDeviceProcessNode
    {
        AudioDeviceProcess process;
        AudioProcessData<float> processData;
        std::uint32_t upstreamLatency = 0U;
        std::uint32_t sumLatency() const
        {
            return process.device()->latencyInSamples() + upstreamLatency;
        }
        inline void doProcess()
        {
            process.process(processData);
        }
        static const char* name() { return "AudioDeviceProcessNode"; }
    };
public:
    AudioDeviceGraphBase();
public:
    const AudioDeviceProcessNode& getMetadataFromNode(const ade::NodeHandle& nodeHandle) const;
    AudioDeviceProcessNode& getMetadataFromNode(ade::NodeHandle& nodeHandle);
    void setMetadataFromNode(ade::NodeHandle& nodeHandle, AudioDeviceProcessNode&& metadata);
public:
    ade::NodeHandle addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData);
    void removeNode(ade::NodeHandle nodeHandle);
    ade::EdgeHandle connect(ade::NodeHandle from, ade::NodeHandle to, std::uint32_t fromChannel, std::uint32_t toChannel);
    void disconnect(ade::EdgeHandle edgeHandle);
public:
    bool latencyCompensationEnabled() const;
    void setLatencyCompensationEnabled();
protected:
    ade::Graph graph_;
    ade::TypedGraph<AudioDeviceProcessNode> typedGraph_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE
