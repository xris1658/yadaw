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

#include <deque>
#include <memory>
#include <set>

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
public:
    AudioDeviceGraph();
private:
    auto& getMetadataFromNode(const ade::NodeHandle& nodeHandle) const;
    auto& getMetadataFromNode(ade::NodeHandle& nodeHandle);
    void setMetadataFromNode(ade::NodeHandle& nodeHandle, AudioDeviceProcessNode&& metadata);
private:
    ade::NodeHandle doAddNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData);
    void doRemoveNode(ade::NodeHandle nodeHandle);
    ade::EdgeHandle doConnect(ade::NodeHandle from, ade::NodeHandle to, std::uint32_t fromChannel, std::uint32_t toChannel);
    void doDisconnect(ade::EdgeHandle edgeHandle);
public:
    ade::NodeHandle addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData);
    void removeNode(ade::NodeHandle nodeHandle);
    ade::EdgeHandle connect(ade::NodeHandle from, ade::NodeHandle to,
        std::uint32_t fromChannel, std::uint32_t toChannel);
    void disconnect(ade::EdgeHandle edgeHandle);
    void disconnect(const std::vector<ade::EdgeHandle>& edgeHandles);
public:
    std::optional<YADAW::Util::TopologicalOrderResult<AudioDeviceProcessNode>> topologicalOrder() const;
private:
    void onSumLatencyChanged(ade::NodeHandle nodeHandle);
    void compensate();
private:
    ade::Graph graph_;
    ade::TypedGraph<AudioDeviceProcessNode> typedGraph_;
    std::vector<std::pair<ade::NodeHandle, std::vector<ade::NodeHandle>>> multiInputNodes_;
    std::set<std::unique_ptr<YADAW::Audio::Util::SampleDelay>> pdc_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
