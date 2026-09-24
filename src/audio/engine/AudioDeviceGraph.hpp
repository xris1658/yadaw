#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH

#include "audio/engine/AudioDeviceGraphBase.hpp"
#include "util/CompilerSpecifics.hpp"
#include "util/TupleUtil.hpp"

#include <tuple>
#include <type_traits>

namespace YADAW::Audio::Engine
{
template<typename T>
concept HasNodeDataConcept  = requires() { std::declval<typename T::NodeData>();  };
template<typename T>
struct HasNodeDataHelper  { static constexpr bool Value = HasNodeDataConcept<T>;  };

template<typename T>
concept HasEdgeDataConcept  = requires() { std::declval<typename T::EdgeData>();  };
template<typename T>
struct HasEdgeDataHelper  { static constexpr bool Value = HasEdgeDataConcept<T>;  };

template<typename T>
concept HasGraphDataConcept = requires() { std::declval<typename T::GraphData>(); };
template<typename T>
struct HasGraphDataHelper { static constexpr bool Value = HasGraphDataConcept<T>; };

template<HasNodeDataConcept  T>
using NodeDataInside  = typename T::NodeData;
template<HasEdgeDataConcept  T>
using EdgeDataInside  = typename T::EdgeData;
template<HasGraphDataConcept T>
using GraphDataInside = typename T::GraphData;

template<typename... Extensions>
class AudioDeviceGraph: public YADAW::Audio::Engine::AudioDeviceGraphBase
{
private:
    using Self = AudioDeviceGraph<Extensions...>;
    using NodeData  = TransformedTuple<NodeDataInside,  FilteredTuple<HasNodeDataHelper,  Extensions...>>::Type;
    using EdgeData  = TransformedTuple<EdgeDataInside,  FilteredTuple<HasEdgeDataHelper,  Extensions...>>::Type;
    using GraphData = TransformedTuple<GraphDataInside, FilteredTuple<HasGraphDataHelper, Extensions...>>::Type;
    template<std::size_t Index = 0>
    ALWAYS_INLINE void afterAddNode(const ade::NodeHandle& nodeHandle)
    {
        std::get<Index>(extensions_).onNodeAdded(nodeHandle);
        if constexpr(Index + 1 < std::tuple_size_v<decltype(extensions_)>)
        {
            afterAddNode<Index + 1>(nodeHandle);
        }
    }
    template<std::size_t Index = 0>
    ALWAYS_INLINE void beforeRemoveNode(const ade::NodeHandle& nodeHandle)
    {
        std::get<Index>(extensions_).onNodeAboutToBeRemoved(nodeHandle);
        if constexpr(Index + 1 < std::tuple_size_v<decltype(extensions_)>)
        {
            beforeRemoveNode<Index + 1>(nodeHandle);
        }
    }
    template<std::size_t Index = 0>
    ALWAYS_INLINE void afterConnect(const ade::EdgeHandle& edgeHandle)
    {
        std::get<Index>(extensions_).onConnected(edgeHandle);
        if constexpr(Index + 1 < std::tuple_size_v<decltype(extensions_)>)
        {
            afterConnect<Index + 1>(edgeHandle);
        }
    }
    template<std::size_t Index = 0>
    ALWAYS_INLINE void beforeDisconnect(const ade::EdgeHandle& edgeHandle)
    {
        std::get<Index>(extensions_).onAboutToBeDisconnected(edgeHandle);
        if constexpr(Index + 1 < std::tuple_size_v<decltype(extensions_)>)
        {
            beforeDisconnect<Index + 1>(edgeHandle);
        }
    }
    static void afterAddNodeCallback(
        AudioDeviceGraphBase& graph, const ade::NodeHandle& nodeHandle)
    {
        auto data = new NodeData();
        auto& audioDeviceGraph = static_cast<Self&>(graph);
        audioDeviceGraph.getNodeData(nodeHandle).data = data;
        audioDeviceGraph.afterAddNode(nodeHandle);
    }
    static void beforeRemoveNodeCallback(
        AudioDeviceGraphBase& graph, const ade::NodeHandle& nodeHandle)
    {
        auto& audioDeviceGraph = static_cast<Self&>(graph);
        audioDeviceGraph.beforeRemoveNode(nodeHandle);
        delete static_cast<NodeData*>(audioDeviceGraph.getNodeData(nodeHandle).data);
    }
    static void afterConnectCallback(
        AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle)
    {
        auto data = new EdgeData();
        auto& audioDeviceGraph = static_cast<Self&>(graph);
        audioDeviceGraph.getEdgeData(edgeHandle).data = data;
        audioDeviceGraph.afterConnect(edgeHandle);
    }
    static void beforeDisconnectCallback(
        AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle)
    {
        auto& audioDeviceGraph = static_cast<Self&>(graph);
        audioDeviceGraph.beforeDisconnect(edgeHandle);
        delete static_cast<EdgeData*>(audioDeviceGraph.getEdgeData(edgeHandle).data);
    }
public:
    explicit AudioDeviceGraph():
        YADAW::Audio::Engine::AudioDeviceGraphBase(),
        extensions_(
            std::make_tuple(
                Extensions(*this)...
            )
        )
    {
        AudioDeviceGraphBase::setAfterAddNodeCallback(&Self::afterAddNodeCallback);
        AudioDeviceGraphBase::setBeforeRemoveNodeCallback(&Self::beforeRemoveNodeCallback);
        AudioDeviceGraphBase::setAfterConnectCallback(&Self::afterConnectCallback);
        AudioDeviceGraphBase::setBeforeDisconnectCallback(&Self::beforeDisconnectCallback);
        getGraphData().data = new GraphData;
    }
    ~AudioDeviceGraph() override
    {
        clear();
        delete static_cast<GraphData*>(getGraphData().data);
    }
public:
    template<typename Extension>
    const Extension& getExtension() const
    {
        return std::get<Extension>(extensions_);
    }
    template<typename Extension>
    Extension& getExtension()
    {
        return std::get<Extension>(extensions_);
    }
    template<HasNodeDataConcept Extension>
    static typename Extension::NodeData& getExtensionNodeData(
        AudioDeviceGraphBase& graph, const ade::NodeHandle& nodeHandle)
    {
        return std::get<typename Extension::NodeData>(
            *static_cast<NodeData*>(graph.getNodeData(nodeHandle).data)
        );
    }
    template<HasEdgeDataConcept Extension>
    static typename Extension::EdgeData& getExtensionEdgeData(
        AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle)
    {
        return std::get<typename Extension::EdgeData>(
            *static_cast<EdgeData*>(graph.getEdgeData(edgeHandle).data)
        );
    }
    template<HasGraphDataConcept Extension>
    static typename Extension::GraphData& getExtensionGraphData(
        AudioDeviceGraphBase& graph)
    {
        return std::get<typename Extension::GraphData>(
            *static_cast<GraphData*>(graph.getGraphData().data)
        );
    }
    template<HasNodeDataConcept Extension>
    static const typename Extension::NodeData& getExtensionNodeData(
        const AudioDeviceGraphBase& graph, const ade::NodeHandle& nodeHandle)
    {
        return std::get<typename Extension::NodeData>(
            *static_cast<NodeData*>(graph.getNodeData(nodeHandle).data)
        );
    }
    template<HasEdgeDataConcept Extension>
    static const typename Extension::EdgeData& getExtensionEdgeData(
        const AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle)
    {
        return std::get<typename Extension::EdgeData>(
            *static_cast<EdgeData*>(graph.getEdgeData(edgeHandle).data)
        );
    }
    template<HasGraphDataConcept Extension>
    static const typename Extension::GraphData& getExtensionGraphData(
        const AudioDeviceGraphBase& graph)
    {
        return std::get<typename Extension::GraphData>(
            *static_cast<GraphData*>(graph.getGraphData().data)
        );
    }
private:
    std::tuple<Extensions...> extensions_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
