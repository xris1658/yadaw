#ifndef YADAW_SRC_AUDIO_UTIL_AUDIOPROCESSDATAPOINTERCONTAINER
#define YADAW_SRC_AUDIO_UTIL_AUDIOPROCESSDATAPOINTERCONTAINER

#include "audio/device/IAudioDevice.hpp"
#include "util/IntegerRange.hpp"

#include <vector>

namespace YADAW::Audio::Util
{
template<typename SampleType>
class AudioProcessDataPointerContainer
{
    using Self = AudioProcessDataPointerContainer<SampleType>;
public:
    AudioProcessDataPointerContainer() = default;
    AudioProcessDataPointerContainer(const Self& rhs)
    {
        setSingleBufferSize(rhs.audioProcessData_.singleBufferSize);
        const auto inputGroupCount = rhs.audioProcessData_.inputGroupCount;
        setInputGroupCount(inputGroupCount);
        FOR_RANGE0(i, inputGroupCount)
        {
            const auto inputCount = rhs.audioProcessData_.inputCounts[i];
            setInputCount(i, inputCount);
            FOR_RANGE0(j, inputCount)
            {
                setInput(i, j, rhs.audioProcessData_.inputs[i][j]);
            }
        }
        const auto outputGroupCount = rhs.audioProcessData_.outputGroupCount;
        setOutputGroupCount(outputGroupCount);
        FOR_RANGE0(i, outputGroupCount)
        {
            const auto outputCount = rhs.audioProcessData_.outputCounts[i];
            setOutputCount(i, outputCount);
            FOR_RANGE0(j, outputCount)
            {
                setOutput(i, j, rhs.audioProcessData_.outputs[i][j]);
            }
        }
    }
    AudioProcessDataPointerContainer(Self&& rhs) noexcept = default;
public:
    void setSingleBufferSize(std::uint32_t singleBufferSize)
    {
        audioProcessData_.singleBufferSize = singleBufferSize;
    }
    void setInputGroupCount(std::uint32_t inputGroupCount)
    {
        inputCounts_.resize(inputGroupCount, 0);
        inputs_.resize(inputGroupCount, {});
        inputs2_.resize(inputGroupCount, {});
        audioProcessData_.inputGroupCount = inputGroupCount;
        audioProcessData_.inputCounts = inputCounts_.data();
        audioProcessData_.inputs = inputs_.data();
    }
    void setOutputGroupCount(std::uint32_t outputGroupCount)
    {
        outputCounts_.resize(outputGroupCount, 0);
        outputs_.resize(outputGroupCount, {});
        outputs2_.resize(outputGroupCount, {});
        audioProcessData_.outputGroupCount = outputGroupCount;
        audioProcessData_.outputCounts = outputCounts_.data();
        audioProcessData_.outputs = outputs_.data();
    }
    void setInputCount(std::uint32_t index, std::uint32_t inputCount)
    {
        inputCounts_[index] = inputCount;
        inputs2_[index].resize(inputCount, {});
        inputs_[index] = inputs2_[index].data();
    }
    void setOutputCount(std::uint32_t index, std::uint32_t outputCount)
    {
        outputCounts_[index] = outputCount;
        outputs2_[index].resize(outputCount, {});
        outputs_[index] = outputs2_[index].data();
    }
    void setInput(std::uint32_t groupIndex, std::uint32_t channelIndex, SampleType* buffer)
    {
        inputs2_[groupIndex][channelIndex] = buffer;
    }
    void setOutput(std::uint32_t groupIndex, std::uint32_t channelIndex, SampleType* buffer)
    {
        outputs2_[groupIndex][channelIndex] = buffer;
    }
    const YADAW::Audio::Device::AudioProcessData<SampleType>& audioProcessData() const
    {
        return audioProcessData_;
    }
private:
    YADAW::Audio::Device::AudioProcessData<SampleType> audioProcessData_;
    std::vector<std::uint32_t> inputCounts_;
    std::vector<std::uint32_t> outputCounts_;
    std::vector<SampleType**> inputs_;
    std::vector<std::vector<SampleType*>> inputs2_;
    std::vector<SampleType**> outputs_;
    std::vector<std::vector<SampleType*>> outputs2_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_AUDIOPROCESSDATAPOINTERCONTAINER
