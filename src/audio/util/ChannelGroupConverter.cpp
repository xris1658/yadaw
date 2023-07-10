#include "ChannelGroupConverter.hpp"

namespace YADAW::Audio::Util
{
using YADAW::Audio::Device::IAudioDevice;
using YADAW::Audio::Device::IAudioChannelGroup;

ChannelGroupConverter::ChannelGroupConverter(
    const IAudioChannelGroup& inputChannelGroup,
    const IAudioChannelGroup& outputChannelGroup):
    inputChannelGroup_(AudioChannelGroup::from(inputChannelGroup)),
    outputChannelGroup_(AudioChannelGroup::from(outputChannelGroup)),
    gain_(outputChannelGroup.channelCount(),
        std::vector<float>(inputChannelGroup.channelCount(), 0.0)
    )
{}

ChannelGroupConverter::~ChannelGroupConverter() noexcept
{}

std::uint32_t ChannelGroupConverter::audioInputGroupCount() const
{
    return 1;
}

std::uint32_t ChannelGroupConverter::audioOutputGroupCount() const
{
    return 1;
}

IAudioDevice::OptionalAudioChannelGroup
    ChannelGroupConverter::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0? OptionalAudioChannelGroup(std::ref(inputChannelGroup_)): std::nullopt;
}

IAudioDevice::OptionalAudioChannelGroup
    ChannelGroupConverter::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0? OptionalAudioChannelGroup(std::ref(outputChannelGroup_)): std::nullopt;
}

std::uint32_t ChannelGroupConverter::latencyInSamples() const
{
    return 0;
}

void ChannelGroupConverter::process(const Device::AudioProcessData<float>& audioProcessData)
{
    for(decltype(outputChannelGroup_.channelCount()) i = 0; i < outputChannelGroup_.channelCount(); ++i)
    {
        for(decltype(audioProcessData.singleBufferSize) j = 0; j < audioProcessData.singleBufferSize; ++j)
        {
            float value = 0.0f;
            for(decltype(inputChannelGroup_.channelCount()) k = 0; k < inputChannelGroup_.channelCount(); ++k)
            {
                value += audioProcessData.inputs[0][k][j] * getGain(k, i);
            }
            audioProcessData.outputs[0][i][j] = value;
        }
    }
}

float ChannelGroupConverter::getGain(std::uint32_t from, std::uint32_t to) const
{
    return gain_[to][from];
}

void ChannelGroupConverter::setGain(std::uint32_t from, std::uint32_t to, float gain)
{
    gain_[to][from] = gain;
}
}