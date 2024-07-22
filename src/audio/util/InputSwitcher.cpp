#include "InputSwitcher.hpp"

#include "util/IntegerRange.hpp"

#include <stdexcept>

namespace YADAW::Audio::Util
{
InputSwitcher::InputSwitcher(std::uint32_t inputCount,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCount):
    inputGroupCount_(inputCount)
{
    if(inputCount == 0)
    {
        throw std::invalid_argument(
            "Input count should be more than zero. Use `BlankGenerator` instead."
        );
    }
    inputChannelGroup_.setChannelGroupType(channelGroupType, channelCount);
    outputChannelGroup_.setChannelGroupType(channelGroupType, channelCount);
}

std::uint32_t InputSwitcher::audioInputGroupCount() const
{
    return inputGroupCount_;
}

std::uint32_t InputSwitcher::audioOutputGroupCount() const
{
    return 1;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    InputSwitcher::audioInputGroupAt(std::uint32_t index) const
{
    return index < inputGroupCount_?
        OptionalAudioChannelGroup(inputChannelGroup_):
        std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    InputSwitcher::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(inputChannelGroup_):
        std::nullopt;
}

std::uint32_t InputSwitcher::latencyInSamples() const
{
    return 0U;
}

void InputSwitcher::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    auto inputIndex = inputIndex_.load(std::memory_order_relaxed);
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        std::memcpy(
            audioProcessData.outputs[0][i],
            audioProcessData.inputs[inputIndex][i],
            audioProcessData.singleBufferSize * sizeof(float)
        );
    }
}

std::uint32_t InputSwitcher::getInputIndex() const
{
    return inputIndex_.load(std::memory_order_acquire);
}

bool InputSwitcher::setInputIndex(std::uint32_t inputIndex)
{
    if(inputIndex < inputGroupCount_)
    {
        inputIndex_.store(inputIndex, std::memory_order_relaxed);
        return true;
    }
    return false;
}
}
