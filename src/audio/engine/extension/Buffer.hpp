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
    struct DataType
    {
        YADAW::Audio::Engine::AudioProcessDataBufferContainer<float> container;
        std::function<BufferSizeChangedCallback> bufferSizeChangedCallback;
    };
public:
    Buffer(AudioDeviceGraphBase& graph,
        DataType&(*getData)(AudioDeviceGraphBase&, const ade::NodeHandle&));
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
        getData(nodeHandle).bufferSizeChangedCallback = std::forward<Func>(callback);
    }
    void resetBufferSizeChangedCallback(const ade::NodeHandle& nodeHandle);
public:
    const DataType& getData(const ade::NodeHandle& nodeHandle) const;
    DataType& getData(const ade::NodeHandle& nodeHandle);
private:
    AudioDeviceGraphBase& graph_;
    DataType&(*getData_)(AudioDeviceGraphBase&, const ade::NodeHandle&);
    std::uint32_t bufferSize_ = 0U;
    YADAW::Util::IntrusivePointer<YADAW::Audio::Util::AudioBufferPool> pool_;
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer> dummyInput_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_EXTENSION_BUFFER
