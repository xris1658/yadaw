#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE

#include "audio/engine/AudioDeviceProcess.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"
#include "audio/util/AudioBufferPool.hpp"
#include "util/ADEUtil.hpp"

#include <ade/node.hpp>
#include <ade/edge.hpp>
#include <ade/graph.hpp>
#include <ade/typed_graph.hpp>
#include <ade/typed_metadata.hpp>

#include <deque>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>

namespace YADAW::Audio::Engine
{
class AudioDeviceGraphBase
{
public:
    struct NodeData
    {
        YADAW::Audio::Engine::AudioDeviceProcess process;
        YADAW::Audio::Engine::AudioProcessDataBufferContainer<float> processData;
        inline void doProcess()
        {
            process.process(processData.audioProcessData());
        }
        static const char* name() { return "NodeData"; }
    };
    struct EdgeData
    {
        std::uint32_t fromChannel;
        std::uint32_t toChannel;
        static const char* name() { return "EdgeData"; }
    };
public:
    AudioDeviceGraphBase(std::uint32_t bufferSize);
    AudioDeviceGraphBase(const AudioDeviceGraphBase&) = delete;
    ~AudioDeviceGraphBase() = default;
public:
    const NodeData& getNodeData(const ade::NodeHandle& nodeHandle) const;
    NodeData& getNodeData(const ade::NodeHandle& nodeHandle);
    const EdgeData& getEdgeData(const ade::EdgeHandle& edgeHandle) const;
    EdgeData& getEdgeData(const ade::EdgeHandle& edgeHandle);
public:
    std::uint32_t bufferSize() const;
    void setBufferSize(std::uint32_t bufferSize);
public:
    ade::NodeHandle addNode(
        YADAW::Audio::Engine::AudioDeviceProcess&& process);
    void removeNode(const ade::NodeHandle& nodeHandle);
    std::optional<ade::EdgeHandle> connect(
        const ade::NodeHandle& fromNode, const ade::NodeHandle& toNode,
        std::uint32_t fromChannel, std::uint32_t toChannel
    );
    void disconnect(const ade::EdgeHandle& edgeHandle);
private:
    ade::Graph graph_;
    ade::TypedGraph<NodeData, EdgeData> typedGraph_;
    std::uint32_t bufferSize_;
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool> pool_;
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer> dummyInput_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE
