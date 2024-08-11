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

void processWithMute(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    FOR_RANGE0(i, audioProcessData.outputGroupCount)
    {
        FOR_RANGE0(j, audioProcessData.outputCounts[i])
        {
            std::memset(
                audioProcessData.outputs[i][j], 0,
                audioProcessData.singleBufferSize * sizeof(float)
            );
        }
    }
}

void processWithoutMute(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    FOR_RANGE0(i, audioProcessData.outputGroupCount)
    {
        FOR_RANGE0(j, audioProcessData.outputCounts[i])
        {
            std::memcpy(
                audioProcessData.outputs[i][j],
                audioProcessData.inputs[i][j],
                audioProcessData.singleBufferSize * sizeof(float)
            );
        }
    }
}

using ProcessFuncType = decltype(&processWithMute);

ProcessFuncType processFuncs[2] = {
    &processWithoutMute,
    &processWithMute
};

void Mute::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    auto func = processFuncs[mute_.load(std::memory_order_acquire)];
    func(audioProcessData);
}

bool Mute::getMute() const
{
    return mute_.load(std::memory_order_acquire);
}

void Mute::setMute(bool mute)
{
    mute_.store(mute, std::memory_order_release);
}
}
