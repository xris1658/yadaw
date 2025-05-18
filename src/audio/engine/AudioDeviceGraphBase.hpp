#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE

#include "audio/engine/AudioDeviceProcess.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"
#include "audio/util/AudioBufferPool.hpp"
#include "util/ADEUtil.hpp"
#include "util/VectorTypes.hpp"

#include <ade/node.hpp>
#include <ade/edge.hpp>
#include <ade/graph.hpp>
#include <ade/typed_graph.hpp>
#include <ade/typed_metadata.hpp>

#include <memory>
#include <optional>

namespace YADAW::Audio::Engine
{
// TODO: Improve API design
// 1. Manage connections by channels instead of channel groups.
//    This requires an updated latency calculation and compensation.
//    In return, we get a more flexible signal routing.
// 2. Return node data on removing nodes.
//    Extending the lifetime of removed nodes makes it easier to synchronize
//    audio and UI threads.
class AudioDeviceGraphBase
{
public:
    struct NodeData
    {
        YADAW::Audio::Engine::AudioDeviceProcess process;
        void* data;
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
    explicit AudioDeviceGraphBase();
    AudioDeviceGraphBase(const AudioDeviceGraphBase&) = delete;
    virtual ~AudioDeviceGraphBase();
public:
    const NodeData& getNodeData(const ade::NodeHandle& nodeHandle) const;
    NodeData& getNodeData(const ade::NodeHandle& nodeHandle);
    const EdgeData& getEdgeData(const ade::EdgeHandle& edgeHandle) const;
    EdgeData& getEdgeData(const ade::EdgeHandle& edgeHandle);
public:
    ade::NodeHandle addNode(
        YADAW::Audio::Engine::AudioDeviceProcess process);
    void removeNode(const ade::NodeHandle& nodeHandle);
    std::optional<ade::EdgeHandle> connect(
        const ade::NodeHandle& fromNode, const ade::NodeHandle& toNode,
        std::uint32_t fromChannel, std::uint32_t toChannel
    );
    void disconnect(const ade::EdgeHandle& edgeHandle);
    void clear();
    auto nodes() const
    {
        return graph_.nodes();
    }
protected:
    using AfterAddNodeCallback = void(AudioDeviceGraphBase& graph, const ade::NodeHandle& nodeHandle);
    using BeforeRemoveNodeCallback = void(AudioDeviceGraphBase& graph, const ade::NodeHandle& nodeHandle);
    using AfterConnectCallback = void(AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle);
    using BeforeDisconnectCallback = void(AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle);
    void setAfterAddNodeCallback(AfterAddNodeCallback* func);
    // This callback WILL be invoked on destructing the graph object.
    // Since those `IAudioDevice` objects associated with nodes are accessible
    // in the callback, the graph MUST NOT survive `IAudioDevice`s in the graph.
    void setBeforeRemoveNodeCallback(BeforeRemoveNodeCallback* func);
    void setAfterConnectCallback(AfterConnectCallback* func);
    // This callback WILL be invoked on destructing the graph object.
    // Since those `IAudioDevice` objects associated with nodes are accessible
    // in the callback, the graph MUST NOT survive `IAudioDevice`s in the graph.
    void setBeforeDisconnectCallback(BeforeDisconnectCallback* func);
    void resetAfterAddNodeCallback();
    void resetBeforeRemoveNodeCallback();
    void resetAfterConnectCallback();
    void resetBeforeDisconnectCallback();
public:
    Vec<Vec<Vec<ade::NodeHandle>, Vec<std::uint32_t, std::uint32_t>>> topologicalSortWithPrev() const;
private:
    ade::Graph graph_;
    ade::TypedGraph<NodeData, EdgeData> typedGraph_;
    AfterAddNodeCallback* afterAddNodeCallback_;
    BeforeRemoveNodeCallback* beforeRemoveNodeCallback_;
    AfterConnectCallback* afterConnectCallback_;
    BeforeDisconnectCallback* beforeDisconnectCallback_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPHBASE
