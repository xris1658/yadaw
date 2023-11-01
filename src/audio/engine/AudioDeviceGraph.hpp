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
    ALWAYS_INLINE void afterAddNodeCallback(const ade::NodeHandle& nodeHandle)
    {
        std::get<Index>(extensions_).onNodeAdded(nodeHandle);
        if constexpr(Index + 1 < std::tuple_size_v<decltype(extensions_)>)
        {
            afterAddNodeCallback<Index + 1>(nodeHandle);
        }
    }
    template<std::size_t Index = 0>
    ALWAYS_INLINE void beforeRemoveNodeCallback(const ade::NodeHandle& nodeHandle)
    {
        std::get<Index>(extensions_).onNodeAboutToBeRemoved(nodeHandle);
        if constexpr(Index + 1 < std::tuple_size_v<decltype(extensions_)>)
        {
            beforeRemoveNodeCallback<Index + 1>(nodeHandle);
        }
    }
    template<std::size_t Index = 0>
    ALWAYS_INLINE void afterConnectCallback(const ade::EdgeHandle& edgeHandle)
    {
        std::get<Index>(extensions_).onConnected(edgeHandle);
        if constexpr(Index + 1 < std::tuple_size_v<decltype(extensions_)>)
        {
            afterConnectCallback(edgeHandle);
        }
    }
    template<std::size_t Index = 0>
    ALWAYS_INLINE void beforeDisconnectCallback(const ade::EdgeHandle& edgeHandle)
    {
        std::get<Index>(extensions_).onAboutToBeDisconnected(edgeHandle);
        if constexpr(Index + 1 < std::tuple_size_v<decltype(extensions_)>)
        {
            beforeDisconnectCallback(edgeHandle);
        }
    }
public:
    explicit AudioDeviceGraph(std::uint32_t bufferSize):
        YADAW::Audio::Engine::AudioDeviceGraphBase(bufferSize),
        extensions_(
            std::make_tuple(
                Extensions(
                    static_cast<AudioDeviceGraphBase&>(*this),
                    getExtensionData<Extensions>
                )...
            )
        )
    {
        AudioDeviceGraphBase::setAfterAddNodeCallback(
            [this](const ade::NodeHandle& nodeHandle)
            {
                auto data = new DataType();
                this->getNodeData(nodeHandle).data = data;
                afterAddNodeCallback(nodeHandle);
            }
        );
        AudioDeviceGraphBase::setBeforeRemoveNodeCallback(
            [this](const ade::NodeHandle& nodeHandle)
            {
                beforeRemoveNodeCallback(nodeHandle);
                delete getNodeData(nodeHandle).data;
            }
        );
        AudioDeviceGraphBase::setAfterConnectCallback(
            [this](const ade::EdgeHandle& edgeHandle)
            {
                afterConnectCallback(edgeHandle);
            }
        );
        AudioDeviceGraphBase::setBeforeRemoveNodeCallback(
            [this](const ade::EdgeHandle& edgeHandle)
            {
                beforeDisconnectCallback(edgeHandle);
            }
        );
    }
    ~AudioDeviceGraph() override
    {
        clear();
    }
public:
    template<typename Extension>
    Extension& getExtension()
    {
        return std::get<Extension>(extensions_);
    }
    template<typename Extension>
    const typename Extension::DataType& doGetExtensionData(
        const ade::NodeHandle& nodeHandle)
    {
        return std::get<typename Extension::DataType>(
            static_cast<DataType>(getNodeData(nodeHandle).data)
        );
    }
    template<typename Extension>
    typename Extension::DataType& doGetExtensionData(
        const ade::NodeHandle& nodeHandle)
    {
        return const_cast<typename Extension::DataType&>(
            static_cast<const Self*>(this)->doGetExtensionData<Extension>(nodeHandle)
        );
    }
private:
    template<typename Extension>
    static const typename Extension::DataType& getExtensionData(
        const AudioDeviceGraphBase& graph,
        const ade::NodeHandle& nodeHandle)
    {
        return static_cast<const Self&>(graph).doGetExtensionData<Extension>(nodeHandle);
    }
    template<typename Extension>
    static typename Extension::DataType& getExtensionData(
        AudioDeviceGraphBase& graph,
        const ade::NodeHandle& nodeHandle)
    {
        return static_cast<Self&>(graph).doGetExtensionData<Extension>(nodeHandle);
    }
private:
    std::tuple<Extensions...> extensions_;
};
}

#endif //YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
