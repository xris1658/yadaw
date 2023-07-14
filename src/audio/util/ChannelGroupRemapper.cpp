#include "ChannelGroupRemapper.hpp"

namespace YADAW::Audio::Util
{
using YADAW::Audio::Device::IAudioDevice;
using YADAW::Audio::Device::IAudioChannelGroup;

ChannelGroupRemapper::ChannelGroupRemapper(
    const IAudioChannelGroup& inputChannelGroup,
    const IAudioChannelGroup& outputChannelGroup):
    inputChannelGroup_(AudioChannelGroup::from(inputChannelGroup)),
    outputChannelGroup_(AudioChannelGroup::from(outputChannelGroup)),
    gain_(outputChannelGroup.channelCount(),
        std::vector<float>(inputChannelGroup.channelCount(), 0.0)
    )
{}

ChannelGroupRemapper::~ChannelGroupRemapper() noexcept
{}

std::uint32_t ChannelGroupRemapper::audioInputGroupCount() const
{
    return 1;
}

std::uint32_t ChannelGroupRemapper::audioOutputGroupCount() const
{
    return 1;
}

IAudioDevice::OptionalAudioChannelGroup
    ChannelGroupRemapper::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0? OptionalAudioChannelGroup(std::ref(inputChannelGroup_)): std::nullopt;
}

IAudioDevice::OptionalAudioChannelGroup
    ChannelGroupRemapper::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0? OptionalAudioChannelGroup(std::ref(outputChannelGroup_)): std::nullopt;
}

std::uint32_t ChannelGroupRemapper::latencyInSamples() const
{
    return 0;
}

void ChannelGroupRemapper::process(const Device::AudioProcessData<float>& audioProcessData)
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

float ChannelGroupRemapper::getGain(std::uint32_t from, std::uint32_t to) const
{
    return gain_[to][from];
}

void ChannelGroupRemapper::setGain(std::uint32_t from, std::uint32_t to, float gain)
{
    gain_[to][from] = gain;
}
}