#include "Summing.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Util
{
Summing::Summing(std::uint32_t inputCount, YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    inputs_.reserve(inputCount);
    FOR_RANGE0(i, inputCount)
    {
        inputs_.emplace_back().setChannelGroupType(channelGroupType, channelCountInGroup);
    }
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
    output_.setMain(true);
}

std::uint32_t Summing::audioInputGroupCount() const
{
    return inputs_.size();
}

uint32_t Summing::audioOutputGroupCount() const
{
    return 1;
}

std::optional<std::reference_wrapper<const Device::IAudioChannelGroup>>
Summing::audioInputGroupAt(std::uint32_t index) const
{
    return index < audioInputGroupCount()? OptionalAudioChannelGroup(std::ref(inputs_[index])): std::nullopt;
}

std::optional<std::reference_wrapper<const Device::IAudioChannelGroup>>
Summing::audioOutputGroupAt(std::uint32_t index) const
{
    return index < 1? OptionalAudioChannelGroup(std::ref(output_)): std::nullopt;
}

uint32_t Summing::latencyInSamples() const
{
    return 0;
}

void Summing::process(const Device::AudioProcessData<float>& audioProcessData)
{
    auto inputCount = inputs_.size();
    for(std::uint32_t i = 0; i < output_.channelCount(); ++i)
    {
        std::memset(audioProcessData.outputs[0][i], 0, sizeof(float) * audioProcessData.singleBufferSize);
    }
    for(std::uint32_t i = 0; i < inputCount; ++i)
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