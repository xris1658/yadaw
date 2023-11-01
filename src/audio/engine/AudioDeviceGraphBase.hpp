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

#include <memory>
#include <optional>

namespace YADAW::Audio::Engine
{
class AudioDeviceGraphBase
{
public:
    struct NodeData
    {
        YADAW::Audio::Engine::AudioDeviceProcess process;
        YADAW::Audio::Engine::AudioProcessDataBufferContainer<float> processData;
        void* data;
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
        void* data;
        static const char* name() { return "EdgeData"; }
    };
public:
    explicit AudioDeviceGraphBase(std::uint32_t bufferSize);
    AudioDeviceGraphBase(const AudioDeviceGraphBase&) = delete;
    virtual ~AudioDeviceGraphBase();
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
    void clear();
protected:
    using AfterAddNodeCallback = void(AudioDeviceGraphBase& graph, const ade::NodeHandle& nodeHandle);
    using BeforeRemoveNodeCallback = void(AudioDeviceGraphBase& graph, const ade::NodeHandle& nodeHandle);
    using AfterConnectCallback = void(AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle);
    using BeforeDisconnectCallback = void(AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle);
    void setAfterAddNodeCallback(std::function<AfterAddNodeCallback>&& func);
    // This callback WILL be invoked on destructing the graph object.
    // Since those `IAudioDevice` objects associated with nodes are accessible
    // in the callback, the graph MUST NOT survive `IAudioDevice`s in the graph.
    void setBeforeRemoveNodeCallback(std::function<BeforeRemoveNodeCallback>&& func);
    void setAfterConnectCallback(std::function<AfterConnectCallback>&& func);
    // This callback WILL be invoked on destructing the graph object.
    // Since those `IAudioDevice` objects associated with nodes are accessible
    // in the callback, the graph MUST NOT survive `IAudioDevice`s in the graph.
    void setBeforeDisconnectCallback(std::function<BeforeDisconnectCallback>&& func);
    void resetAfterAddNodeCallback();
    void resetBeforeRemoveNodeCallback();
    void resetAfterConnectCallback();
    void resetBeforeDisconnectCallback();
public:
    std::vector<std::vector<std::vector<ade::NodeHandle>>> topologicalSort() const;
private:
    ade::Graph graph_;
    ade::TypedGraph<NodeData, EdgeData> typedGraph_;
    std::uint32_t bufferSize_;
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool> pool_;
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer> dummyInput_;
    std::function<AfterAddNodeCallback> afterAddNodeCallback_;
    std::function<BeforeRemoveNodeCallback> beforeRemoveNodeCallback_;
    std::function<AfterConnectCallback> afterConnectCallback_;
    std::function<BeforeDisconnectCallback> beforeDisconnectCallback_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE
