#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH

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
class AudioDeviceGraph
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
    struct EdgeData
    {
        ade::NodeHandle toNode;
        std::uint32_t fromChannel;
        std::uint32_t toChannel;
        static const char* name() { return "EdgeData"; }
    };
    using TopologicalSortResult =
        std::vector<std::vector<std::vector<AudioDeviceProcessNode>>>;
public:
    AudioDeviceGraph(bool latencyCompensationEnabled = true);
public:
    const AudioDeviceProcessNode& getMetadata(const ade::NodeHandle& nodeHandle) const;
    const EdgeData& getMetadata(const ade::EdgeHandle& edgeHandle);
    AudioDeviceProcessNode& getMetadata(ade::NodeHandle& nodeHandle);
    void setMetadata(ade::NodeHandle& nodeHandle, AudioDeviceProcessNode&& metadata);
private:
    ade::NodeHandle doAddNode(AudioDeviceProcess&& process, const AudioProcessData<float>& audioProcessData);
    ade::NodeHandle doAddNode(AudioDeviceProcess&& process, AudioProcessData<float>& audioProcessData);
    void doRemoveNode(ade::NodeHandle nodeHandle);
    ade::EdgeHandle doConnect(ade::NodeHandle from, ade::NodeHandle to, EdgeData edgeData);
    void doDisconnect(ade::EdgeHandle edgeHandle);
    void doAddLatencyCompensation(const ade::NodeHandle& nodeHandle);
public:
    ade::NodeHandle addNode(AudioDeviceProcess&& process, const AudioProcessData<float>& audioProcessData);
    ade::NodeHandle addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData);
    void removeNode(ade::NodeHandle nodeHandle);
    // Connects two nodes if no "to -> from" is found. Returns an invalid
    // `ade::EdgeHandle` otherwise.
    ade::EdgeHandle connect(ade::NodeHandle from, ade::NodeHandle to,
        std::uint32_t fromChannel, std::uint32_t toChannel);
    // Remove an edge. It does nothing if `edgeHandle` is invalid.
    void disconnect(ade::EdgeHandle edgeHandle);
    // Remove edges. It does nothing while removing invalid `ade::EdgeHandle`s.
    void disconnect(const std::vector<ade::EdgeHandle>& edgeHandles);
public:
    TopologicalSortResult topologicalSort() const;
public:
    void onSumLatencyChanged(ade::NodeHandle nodeHandle);
private:
    void compensate(bool latencyCompensationEnabled);
public:
    bool latencyCompensationEnabled() const;
    void setLatencyCompensationEnabled(bool enabled);
private:
    ade::Graph graph_;
    ade::TypedGraph<AudioDeviceProcessNode, EdgeData> typedGraph_;
    std::unordered_map<
        ade::NodeHandle,
        std::vector<
            std::pair<
                ade::NodeHandle,
                YADAW::Audio::Util::SampleDelay
            >
        >,
        ade::HandleHasher<ade::Node>
    >
    multiInputs_;
    std::unordered_set<
        ade::NodeHandle,
        ade::HandleHasher<ade::Node>
    >
    multiOutputs_;
    bool latencyCompensationEnabled_ = true;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
