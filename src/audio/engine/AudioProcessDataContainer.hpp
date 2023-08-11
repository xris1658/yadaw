#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIOPROCESSDATACONTAINER
#define YADAW_SRC_AUDIO_ENGINE_AUDIOPROCESSDATACONTAINER

#include "audio/util/AudioProcessDataPointerContainer.hpp"
#include "audio/util/AudioBufferPool.hpp"
#include "util/VectorUtil.hpp"

namespace YADAW::Audio::Engine
{
template<typename SampleType>
class AudioProcessDataContainer
{
public:
    AudioProcessDataContainer(std::shared_ptr<YADAW::Audio::Util::AudioBufferPool> audioBufferPool):
        audioBufferPool_(std::move(audioBufferPool))
    {
        container_.setSingleBufferSize(audioBufferPool_->singleBufferByteSize() / sizeof(SampleType));
    }
public:
    void setInputGroupCount(std::uint32_t inputGroupCount)
    {
        container_.setInputGroupCount(inputGroupCount);
        YADAW::Util::resizeVector(inputBuffers_, inputGroupCount);
    }
    void setOutputGroupCount(std::uint32_t outputGroupCount)
    {
        container_.setOutputGroupCount(outputGroupCount);
        YADAW::Util::resizeVector(outputBuffers_, outputGroupCount);
    }
    void setInputCount(std::uint32_t index, std::uint32_t inputCount)
    {
        container_.setInputCount(index, inputCount);
        auto& inputBuffers = inputBuffers_[index];
        YADAW::Util::resizeVector(inputBuffers, inputCount);
        std::size_t i = 0;
        for(auto& inputBuffer: inputBuffers)
        {
            if(!inputBuffer.pointer())
            {
                inputBuffer = audioBufferPool_->lend();
                container_.setInput(index, i, reinterpret_cast<SampleType*>(inputBuffer.pointer()));
            }
            ++i;
        }
    }
    void setOutputCount(std::uint32_t index, std::uint32_t outputCount)
    {
        container_.setOutputCount(index, outputCount);
        auto& outputBuffers = outputBuffers_[index];
        YADAW::Util::resizeVector(outputBuffers, outputCount);
        std::size_t i = 0;
        for(auto& outputBuffer: outputBuffers_[index])
        {
            if(!outputBuffer.pointer())
            {
                outputBuffer = audioBufferPool_->lend();
                container_.setOutput(index, i, reinterpret_cast<SampleType*>(outputBuffer.pointer()));
            }
            ++i;
        }
    }
    const YADAW::Audio::Device::AudioProcessData<SampleType>& audioProcessData() const
    {
        return container_.audioProcessData();
    }
private:
    std::shared_ptr<YADAW::Audio::Util::AudioBufferPool> audioBufferPool_;
    YADAW::Audio::Util::AudioProcessDataPointerContainer<SampleType> container_;
    std::vector<std::vector<YADAW::Audio::Util::AudioBufferPool::Buffer>> inputBuffers_;
    std::vector<std::vector<YADAW::Audio::Util::AudioBufferPool::Buffer>> outputBuffers_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIOPROCESSDATACONTAINER
