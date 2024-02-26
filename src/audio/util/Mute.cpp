#include "Mute.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Util
{
Mute::Mute(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    input_.setChannelGroupType(channelGroupType, channelCountInGroup);
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
}

std::uint32_t Mute::audioInputGroupCount() const
{
    return 1;
}

std::uint32_t Mute::audioOutputGroupCount() const
{
    return 1;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    Mute::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(input_):
        std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    Mute::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(output_):
        std::nullopt;
}

std::uint32_t Mute::latencyInSamples() const
{
    return 0;
}

constexpr float factor[2] = {1.0f, 0.0f};

void Mute::process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    if(muteUpdated_)
    {
        muteInCallback_ = mute_;
        muteUpdated_.store(false, std::memory_order::memory_order_release);
    }
    FOR_RANGE0(i, audioProcessData.inputCounts[0])
    {
        FOR_RANGE0(j, audioProcessData.singleBufferSize)
        {
            audioProcessData.outputs[0][i][j] = audioProcessData.inputs[0][i][j] * factor[muteInCallback_];
        }
    }
}

bool Mute::mute() const
{
    while(muteUpdated_.load(std::memory_order::memory_order_acquire)) {}
    return mute_;
}

void Mute::setMute(bool mute)
{
    while(muteUpdated_.load(std::memory_order::memory_order_acquire)) {}
    mute_ = mute;
    muteUpdated_.store(true, std::memory_order::memory_order_release);
}
}