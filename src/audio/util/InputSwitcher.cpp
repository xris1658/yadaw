#include "InputSwitcher.hpp"

#include "native/CPU.hpp"
#include "util/IntegerRange.hpp"

#include <stdexcept>

#if YADAW_CPUARCH_X64
#include <xmmintrin.h>
#endif

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
#if YADAW_CPUARCH_X64
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        constexpr auto floatCount = sizeof(__m128) / sizeof(float);
        auto alignCount = audioProcessData.singleBufferSize / floatCount;
        auto alignedInput = reinterpret_cast<__m128*>(audioProcessData.inputs[inputIndex][i]);
        auto output = audioProcessData.outputs[0][i];
        FOR_RANGE0(j, alignCount)
        {
            _mm_store_ps(
                output + j * floatCount,
                alignedInput[j]
            );
        }
        std::memcpy(
            output + alignCount * floatCount,
            alignedInput + alignCount,
            audioProcessData.singleBufferSize - alignCount * floatCount
        );
    }
#else
    doProcess<false>(audioProcessData, inputIndex);
#endif
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
        std::all_of(
            audioProcessData.inputs[inputIndex],
            audioProcessData.inputs[inputIndex] + audioProcessData.inputCounts[inputIndex],
            [](float* buffer)
            {
                return (reinterpret_cast<std::uint64_t>(buffer) & 0x0000000F) == 0;
            }
        ) &&
        std::all_of(
            audioProcessData.outputs[0],
            audioProcessData.outputs[0] + audioProcessData.outputCounts[0],
            [](float* buffer)
            {
                return (reinterpret_cast<std::uint64_t>(buffer) & 0x0000000F) == 0;
            }
        );
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
