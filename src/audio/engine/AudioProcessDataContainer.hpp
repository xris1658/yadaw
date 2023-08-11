#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIOPROCESSDATACONTAINER
#define YADAW_SRC_AUDIO_ENGINE_AUDIOPROCESSDATACONTAINER

#include "audio/util/AudioProcessDataPointerContainer.hpp"
#include "audio/util/AudioBufferPool.hpp"

namespace YADAW::Audio::Engine
{
template<typename SampleType>
class AudioProcessDataContainer
{
public:
    AudioProcessDataContainer(std::shared_ptr<YADAW::Audio::Util::AudioBufferPool> audioBufferPool):
        audioBufferPool_(std::move(audioBufferPool))
    {
        container_.audioProcessData().singleBufferSize = audioBufferPool_->singleBufferByteSize() / sizeof(SampleType);
    }
public:
    void setInputGroupCount(std::uint32_t inputGroupCount)
    {
        container_.setInputGroupCount(inputGroupCount);
        inputBuffers_.resize(inputGroupCount, {});
    }
    void setOutputGroupCount(std::uint32_t outputGroupCount)
    {
        container_.setOutputGroupCount(outputGroupCount);
        outputBuffers_.resize(outputGroupCount, {});
    }
    void setInputCount(std::uint32_t index, std::uint32_t inputCount)
    {
        container_.setInputCount(index, inputCount);
        inputBuffers_[index].resize(inputCount, {});
        std::size_t i = 0;
        for(auto& inputBuffer: inputBuffers_[index])
        {
            if(!inputBuffer.pointer())
            {
                inputBuffer = audioBufferPool_->lend();
                container_.setInput(index, i, inputBuffer.pointer());
            }
            ++i;
        }
    }
    void setOutputCount(std::uint32_t index, std::uint32_t outputCount)
    {
        container_.setOutputCount(index, outputCount);
        outputBuffers_[index].resize(outputCount, {});
        std::size_t i = 0;
        for(auto& outputBuffer: outputBuffers_[index])
        {
            if(!outputBuffer.pointer())
            {
                outputBuffer = audioBufferPool_->lend();
                container_.setOutput(index, i, outputBuffer.pointer());
            }
            ++i;
        }
    }
private:
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool> audioBufferPool_;
    YADAW::Audio::Util::AudioProcessDataPointerContainer<SampleType> container_;
    std::vector<std::vector<YADAW::Audio::Util::AudioBufferPool::Buffer>> inputBuffers_;
    std::vector<std::vector<YADAW::Audio::Util::AudioBufferPool::Buffer>> outputBuffers_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIOPROCESSDATACONTAINER
