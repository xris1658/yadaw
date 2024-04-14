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

void Mute::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    auto automation = muteAutomation_.load(std::memory_order::memory_order_acquire);
    (this->*processFuncs[automation != nullptr])(audioProcessData, automation);
}

void Mute::processWithoutAutomation(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    const YADAW::Audio::Base::Automation* automation)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        std::memcpy(
            audioProcessData.outputs[0][i],
            audioProcessData.inputs[0][i],
            sizeof(float) * audioProcessData.singleBufferSize
        );
    }
}

static constexpr float factor[2] = {1.0f, 0.0f};

void Mute::processWithAutomation(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    const YADAW::Audio::Base::Automation* automation)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        auto in = audioProcessData.inputs[0][i];
        auto out = audioProcessData.outputs[0][i];
        FOR_RANGE0(j, audioProcessData.singleBufferSize)
        {
            out[j] = in[j] * factor[(*automation)(time_ + j) != 0.0f];
        }
    }
}

}