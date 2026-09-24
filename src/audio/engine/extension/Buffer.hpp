#ifndef YADAW_SRC_AUDIO_ENGINE_EXTENSION_BUFFER
#define YADAW_SRC_AUDIO_ENGINE_EXTENSION_BUFFER

#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"

#include <functional>

namespace YADAW::Audio::Engine::Extension
{
class Buffer
{
public:
    using BufferSizeChangedCallback = void(std::uint32_t newBufferSize);
    class GraphData
    {
        friend class Buffer;
        std::uint32_t bufferSize = 0U;
        YADAW::Util::IntrusivePointer<YADAW::Audio::Util::AudioBufferPool> pool;
        std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer> dummyInput;
    };
    struct NodeData
    {
        YADAW::Audio::Engine::AudioProcessDataBufferContainer<float> container;
        std::function<BufferSizeChangedCallback> bufferSizeChangedCallback;
    };
private:
    Buffer(AudioDeviceGraphBase& graph);
public:
    template<typename... Extensions>
    Buffer(AudioDeviceGraph<Extensions...>& graph):
        Buffer(static_cast<AudioDeviceGraphBase&>(graph))
    {
        getNodeData_  = static_cast<decltype(getNodeData_) >(
            AudioDeviceGraph<Extensions...>::template getExtensionNodeData <Buffer>
        );
        getGraphData_ = static_cast<decltype(getGraphData_)>(
            AudioDeviceGraph<Extensions...>::template getExtensionGraphData<Buffer>
        );
    }
public:
    void onNodeAdded(const ade::NodeHandle& nodeHandle);
    void onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle);
    void onConnected(const ade::EdgeHandle& edgeHandle);
    void onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle);
public:
    std::uint32_t bufferSize() const;
    void setBufferSize(std::uint32_t bufferSize);
    template<typename Func>
    void setBufferSizeChangedCallback(const ade::NodeHandle& nodeHandle, Func&& callback)
    {
        getNodeData(nodeHandle).bufferSizeChangedCallback = std::forward<Func>(callback);
    }
    void resetBufferSizeChangedCallback(const ade::NodeHandle& nodeHandle);
public:
    const NodeData& getNodeData(const ade::NodeHandle& nodeHandle) const;
    NodeData& getNodeData(const ade::NodeHandle& nodeHandle);
private:
    AudioDeviceGraphBase& graph_;
    NodeData& (*getNodeData_) (AudioDeviceGraphBase&, const ade::NodeHandle&);
    GraphData&(*getGraphData_)(AudioDeviceGraphBase&);
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_EXTENSION_BUFFER
