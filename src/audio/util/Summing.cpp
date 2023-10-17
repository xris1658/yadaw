#include "Summing.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Util
{
using YADAW::Audio::Device::AudioProcessData;
using YADAW::Audio::Device::IAudioDevice;

Summing::Summing(
    std::uint32_t inputCount,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup):
    inputCount_(inputCount)
{
    inputs_.setChannelGroupType(channelGroupType, channelCountInGroup);
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
    output_.setMain(true);
}

std::uint32_t Summing::audioInputGroupCount() const
{
    return inputCount_;
}

std::uint32_t Summing::audioOutputGroupCount() const
{
    return 1;
}

IAudioDevice::OptionalAudioChannelGroup Summing::audioInputGroupAt(
    std::uint32_t index) const
{
    return index < audioInputGroupCount()?
        OptionalAudioChannelGroup(std::ref(inputs_)):
        std::nullopt;
}

IAudioDevice::OptionalAudioChannelGroup Summing::audioOutputGroupAt(
    std::uint32_t index) const
{
    return index < 1?
        OptionalAudioChannelGroup(std::ref(output_)):
        std::nullopt;
}

std::uint32_t Summing::latencyInSamples() const
{
    return 0;
}

void Summing::process(const AudioProcessData<float>& audioProcessData)
{
    for(std::uint32_t i = 0; i < output_.channelCount(); ++i)
    {
        std::memset(audioProcessData.outputs[0][i], 0, sizeof(float) * audioProcessData.singleBufferSize);
    }
    for(std::uint32_t i = 0; i < inputCount_; ++i)
    {
        for(std::uint32_t j = 0; j < output_.channelCount(); ++j)
        {
            for(std::uint32_t k = 0; k < audioProcessData.singleBufferSize; ++k)
            {
                audioProcessData.outputs[0][j][k] += audioProcessData.inputs[i][j][k];
            }
        }
    }
}
}