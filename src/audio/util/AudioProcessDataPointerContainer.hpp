#ifndef YADAW_SRC_AUDIO_UTIL_AUDIOPROCESSDATAPOINTERCONTAINER
#define YADAW_SRC_AUDIO_UTIL_AUDIOPROCESSDATAPOINTERCONTAINER

#include "audio/device/IAudioDevice.hpp"

#include <vector>

namespace YADAW::Audio::Util
{
template<typename SampleType>
class AudioProcessDataPointerContainer
{
public:
    void setInputGroupCount(std::uint32_t inputGroupCount)
    {
        inputCounts_.resize(inputGroupCount, 0);
        inputs_.resize(inputGroupCount, {});
        inputs2_.resize(inputGroupCount, {});
        audioProcessData_.inputCounts = inputCounts_.data();
        audioProcessData_.inputs = inputs_.data();
    }
    void setOutputGroupCount(std::uint32_t outputGroupCount)
    {
        outputCounts_.resize(outputGroupCount, 0);
        outputs_.resize(outputGroupCount, {});
        outputs2_.resize(outputGroupCount, {});
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
