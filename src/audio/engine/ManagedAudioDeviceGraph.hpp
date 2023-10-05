#ifndef YADAW_SRC_AUDIO_ENGINE_MANAGEDAUDIODEVICEGRAPH
#define YADAW_SRC_AUDIO_ENGINE_MANAGEDAUDIODEVICEGRAPH

#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"
#include "audio/util/AudioBufferPool.hpp"

#include <memory>
#include <unordered_map>

namespace YADAW::Audio::Engine
{
class ManagedAudioDeviceGraph
{
public:
    ManagedAudioDeviceGraph(
        std::uint32_t bufferSize,
        bool latencyCompensationEnabled = true);
public:
    ade::NodeHandle addNode(YADAW::Audio::Engine::AudioDeviceProcess process);
    void removeNode(ade::NodeHandle nodeHandle);
    ade::EdgeHandle connect(ade::NodeHandle from, ade::NodeHandle to,
        std::uint32_t fromChannel, std::uint32_t toChannel);
    void disconnect(ade::EdgeHandle edgeHandle);
public:
    YADAW::Audio::Engine::AudioDeviceGraph::TopologicalSortResult topologicalSort() const;
public:
    void onSumLatencyChanged(ade::NodeHandle nodeHandle);
public:
    std::uint32_t bufferSize() const;
    bool latencyCompensationEnabled() const;
    void setParams(std::uint32_t bufferSize, bool latencyCompensationEnabled);
private:
    YADAW::Audio::Engine::AudioDeviceGraph graph_;
    std::uint32_t bufferSize_;
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool> pool_;
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer> dummyInput_;
    std::unordered_map<
        ade::NodeHandle,
        YADAW::Audio::Engine::AudioProcessDataBufferContainer<float>,
        ade::HandleHasher<ade::Node>
    >
    processData_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_MANAGEDAUDIODEVICEGRAPH
