#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIOPROCESSDATABUFFERCONTAINER
#define YADAW_SRC_AUDIO_ENGINE_AUDIOPROCESSDATABUFFERCONTAINER

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioBufferPool.hpp"
#include "audio/util/AudioProcessDataPointerContainer.hpp"

#include <memory>
#include <vector>

namespace YADAW::Audio::Engine
{
template<typename SampleType>
class AudioProcessDataBufferContainer
{
    using Self = AudioProcessDataBufferContainer;
public:
    AudioProcessDataBufferContainer() = default;
    AudioProcessDataBufferContainer(const Self& rhs) = default;
    AudioProcessDataBufferContainer(Self&& rhs) noexcept = default;
    ~AudioProcessDataBufferContainer() = default;
public:
    void setSingleBufferSize(std::uint32_t singleBufferSize)
    {
        container_.setSingleBufferSize(singleBufferSize);
    }
    void setInputGroupCount(std::uint32_t inputGroupCount)
    {
        container_.setInputGroupCount(inputGroupCount);
        inputBuffers_.resize(inputGroupCount);
    }
    void setOutputGroupCount(std::uint32_t outputGroupCount)
    {
        container_.setOutputGroupCount(outputGroupCount);
        outputBuffers_.resize(outputGroupCount);
    }
    void setInputCount(std::uint32_t groupIndex, std::uint32_t inputCount)
    {
        container_.setInputCount(groupIndex, inputCount);
        inputBuffers_[groupIndex].resize(inputCount);
    }
    void setOutputCount(std::uint32_t groupIndex, std::uint32_t outputCount)
    {
        container_.setOutputCount(groupIndex, outputCount);
        outputBuffers_[groupIndex].resize(outputCount);
    }
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer>
        inputBuffer(std::uint32_t groupIndex, std::uint32_t channelIndex)
    {
        return inputBuffers_[groupIndex][channelIndex];
    }
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer>
        outputBuffer(std::uint32_t groupIndex, std::uint32_t channelIndex)
    {
        return outputBuffers_[groupIndex][channelIndex];
    }
    void setInputBuffer(std::uint32_t groupIndex, std::uint32_t channelIndex,
        std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer> buffer)
    {
        container_.setInput(groupIndex, channelIndex,
            reinterpret_cast<SampleType*>(buffer->pointer()));
        inputBuffers_[groupIndex][channelIndex] = std::move(buffer);
    }
    void setOutputBuffer(std::uint32_t groupIndex, std::uint32_t channelIndex,
        std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer> buffer)
    {
        container_.setOutput(groupIndex, channelIndex,
            reinterpret_cast<SampleType*>(buffer->pointer()));
        outputBuffers_[groupIndex][channelIndex] = std::move(buffer);
    }
    const YADAW::Audio::Device::AudioProcessData<SampleType>& audioProcessData() const
    {
        return container_.audioProcessData();
    }
private:
    YADAW::Audio::Util::AudioProcessDataPointerContainer<SampleType> container_;
    std::vector<std::vector<std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer>>> inputBuffers_;
    std::vector<std::vector<std::shared_ptr<YADAW::Audio::Util::AudioBufferPool::Buffer>>> outputBuffers_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIOPROCESSDATABUFFERCONTAINER
