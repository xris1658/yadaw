#include "BlankReceiver.hpp"

namespace YADAW::Audio::Mixer
{
BlankReceiver::BlankReceiver(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup):
    input_()
{
    input_.setChannelGroupType(channelGroupType, channelCountInGroup);
}

std::uint32_t BlankReceiver::audioInputGroupCount() const
{
    return 1;
}

std::uint32_t BlankReceiver::audioOutputGroupCount() const
{
    return 0;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
BlankReceiver::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(std::ref(input_)):
        std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
BlankReceiver::audioOutputGroupAt(std::uint32_t index) const
{
    return std::nullopt;
}

std::uint32_t BlankReceiver::latencyInSamples() const
{
    return 0;
}

void BlankReceiver::process(
    const Device::AudioProcessData<float>& audioProcessData)
{}

}