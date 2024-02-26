#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH

#include "audio/engine/AudioDeviceGraphBase.hpp"
#include "util/CompilerSpecifics.hpp"

#include <list>

namespace YADAW::Audio::Engine
{
template<typename... Extensions>
class AudioDeviceGraph: public YADAW::Audio::Engine::AudioDeviceGraphBase
{
private:
    using Self = AudioDeviceGraph<Extensions...>;
    using DataType = std::tuple<typename Extensions::DataType...>;
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
        auto data = new DataType();
        auto& audioDeviceGraph = static_cast<Self&>(graph);
        audioDeviceGraph.getNodeData(nodeHandle).data = data;
        audioDeviceGraph.afterAddNode(nodeHandle);
    }
    static void beforeRemoveNodeCallback(
        AudioDeviceGraphBase& graph, const ade::NodeHandle& nodeHandle)
    {
        auto& audioDeviceGraph = static_cast<Self&>(graph);
        audioDeviceGraph.beforeRemoveNode(nodeHandle);
        delete static_cast<DataType*>(audioDeviceGraph.getNodeData(nodeHandle).data);
    }
    static void afterConnectCallback(
        AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle)
    {
        auto& audioDeviceGraph = static_cast<Self&>(graph);
        audioDeviceGraph.afterConnect(edgeHandle);
    }
    static void beforeDisconnectCallback(
        AudioDeviceGraphBase& graph, const ade::EdgeHandle& edgeHandle)
    {
        auto& audioDeviceGraph = static_cast<Self&>(graph);
        audioDeviceGraph.beforeDisconnect(edgeHandle);
    }
public:
    explicit AudioDeviceGraph():
        YADAW::Audio::Engine::AudioDeviceGraphBase(),
        extensions_(
            std::make_tuple(
                Extensions(
                    static_cast<AudioDeviceGraphBase&>(*this),
                    getExtensionData<Extensions>
                )...
            )
        )
    {
        AudioDeviceGraphBase::setAfterAddNodeCallback(&Self::afterAddNodeCallback);
        AudioDeviceGraphBase::setBeforeRemoveNodeCallback(&Self::beforeRemoveNodeCallback);
        AudioDeviceGraphBase::setAfterConnectCallback(&Self::afterConnectCallback);
        AudioDeviceGraphBase::setBeforeDisconnectCallback(&Self::beforeDisconnectCallback);
    }
    ~AudioDeviceGraph() override
    {
        clear();
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
    template<typename Extension>
    const typename Extension::DataType& getExtensionData(
        const ade::NodeHandle& nodeHandle) const
    {
        return std::get<typename Extension::DataType>(
            *static_cast<DataType*>(getNodeData(nodeHandle).data)
        );
    }
    template<typename Extension>
    typename Extension::DataType& getExtensionData(
        const ade::NodeHandle& nodeHandle)
    {
        return const_cast<typename Extension::DataType&>(
            static_cast<const Self*>(this)->getExtensionData<Extension>(nodeHandle)
        );
    }
private:
    template<typename Extension>
    static const typename Extension::DataType& getExtensionData(
        const AudioDeviceGraphBase& graph,
        const ade::NodeHandle& nodeHandle)
    {
        return static_cast<const Self&>(graph).getExtensionData<Extension>(nodeHandle);
    }
    template<typename Extension>
    static typename Extension::DataType& getExtensionData(
        AudioDeviceGraphBase& graph,
        const ade::NodeHandle& nodeHandle)
    {
        return static_cast<Self&>(graph).getExtensionData<Extension>(nodeHandle);
    }
private:
    std::tuple<Extensions...> extensions_;
};
}

#endif //YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
