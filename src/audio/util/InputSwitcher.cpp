#include "InputSwitcher.hpp"

#include "util/IntegerRange.hpp"

#include <stdexcept>

#include <xmmintrin.h>

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

using ProcessFunc = void(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    std::uint32_t inputIndex);

template<bool Aligned>
void doProcess(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    std::uint32_t inputIndex);

template<>
void doProcess<false>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    std::uint32_t inputIndex)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        std::memcpy(
            audioProcessData.outputs[0][i],
            audioProcessData.inputs[inputIndex][i],
            audioProcessData.singleBufferSize * sizeof(float)
        );
    }
}

template<>
void doProcess<true>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    std::uint32_t inputIndex)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        auto alignCount = audioProcessData.singleBufferSize * sizeof(float) / sizeof(__m128);
        auto alignedInput = reinterpret_cast<__m128*>(audioProcessData.inputs[inputIndex][i]);
        auto output = audioProcessData.outputs[0][i];
        FOR_RANGE0(j, alignCount)
        {
            _mm_store_ps(
                output + j * sizeof(__m128) / sizeof(float),
                alignedInput[j]
            );
        }
    }
}

ProcessFunc* const processFuncs[2] = {
    &doProcess<false>,
    &doProcess<true>,
};

void InputSwitcher::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    auto inputIndex = inputIndex_.load(std::memory_order_relaxed);
    bool aligned =
        (reinterpret_cast<std::uint64_t>(audioProcessData.outputs[0][0]) & 0x0000000F) == 0
     && (reinterpret_cast<std::uint64_t>(audioProcessData.inputs [inputIndex][0]) & 0x0000000F) == 0;
    processFuncs[aligned](audioProcessData, inputIndex);
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
