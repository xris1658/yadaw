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
    AudioDeviceGraph();
public:
    const AudioDeviceProcessNode& getMetadataFromNode(const ade::NodeHandle& nodeHandle) const;
    AudioDeviceProcessNode& getMetadataFromNode(ade::NodeHandle& nodeHandle);
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
public:
    void onSumLatencyChanged(ade::NodeHandle nodeHandle);
private:
    void compensate();
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
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
