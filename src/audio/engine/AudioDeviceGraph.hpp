#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH

#include "audio/engine/AudioDeviceProcess.hpp"
#include "audio/engine/AudioDeviceGraphBase.hpp"
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
class AudioDeviceGraph: private AudioDeviceGraphBase
{
public:
    using AudioDeviceProcessNode = AudioDeviceGraphBase::AudioDeviceProcessNode;
public:
    AudioDeviceGraph();
public:
    const AudioDeviceProcessNode& getMetadataFromNode(const ade::NodeHandle& nodeHandle) const;
    AudioDeviceProcessNode& getMetadataFromNode(ade::NodeHandle& nodeHandle);
    void setMetadataFromNode(ade::NodeHandle& nodeHandle, AudioDeviceProcessNode&& metadata);
public:
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
    std::optional<YADAW::Util::TopologicalOrderResult<AudioDeviceProcessNode>> topologicalOrder() const;
public:
    void onSumLatencyChanged(ade::NodeHandle nodeHandle);
private:
    void compensate(bool latencyCompensationEnabled);
public:
    bool latencyCompensationEnabled() const;
    void setLatencyCompensationEnabled(bool enabled);
private:
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
