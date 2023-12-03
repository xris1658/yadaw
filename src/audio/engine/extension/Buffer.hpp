#ifndef YADAW_SRC_AUDIO_ENGINE_EXTENSION_BUFFER
#define YADAW_SRC_AUDIO_ENGINE_EXTENSION_BUFFER

#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioProcessDataBufferContainer.hpp"

namespace YADAW::Audio::Engine::Extension
{
class Buffer
{
public:
    struct DataType
    {
        YADAW::Audio::Engine::AudioProcessDataBufferContainer<float> container;
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
private:
    AudioDeviceGraphBase& graph_;
    DataType&(*getData_)(AudioDeviceGraphBase&, const ade::NodeHandle&);
    std::uint32_t bufferSize_ = 0U;
    YADAW::Util::IntrusivePointer<YADAW::Audio::Util::AudioBufferPool> pool_;
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer> dummyInput_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_EXTENSION_BUFFER
