#include "BlankGenerator.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Mixer
{
BlankGenerator::BlankGenerator(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup):
    output_()
{
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
}

std::uint32_t BlankGenerator::audioInputGroupCount() const
{
    return 0;
}

std::uint32_t BlankGenerator::audioOutputGroupCount() const
{
    return 1;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    BlankGenerator::audioInputGroupAt(std::uint32_t index) const
{
    return std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    BlankGenerator::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(std::ref(output_)):
        std::nullopt;
}

std::uint32_t BlankGenerator::latencyInSamples() const
{
    return 0;
}

void BlankGenerator::process(
    const Device::AudioProcessData<float>& audioProcessData)
{
    FOR_RANGE0(i, output_.channelCount())
    {
        std::memset(audioProcessData.outputs[0][i], 0,
            sizeof(float) / sizeof(int) * audioProcessData.singleBufferSize);
    }
}

uint32_t BlankGenerator::audioChannelMapCount() const
{
    return 0;
}

YADAW::Audio::Device::IAudioDevice::OptionalChannelMap
    BlankGenerator::audioChannelMapAt(std::uint32_t index) const
{
    return std::nullopt;
}
}