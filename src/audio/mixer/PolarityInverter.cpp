#include "PolarityInverter.hpp"

#include "util/IntegerRange.hpp"

#include <immintrin.h>

namespace YADAW::Audio::Mixer
{
PolarityInverter::PolarityInverter(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    input_.setChannelGroupType(channelGroupType, channelCountInGroup);
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
}

PolarityInverter::~PolarityInverter()
{
}

std::uint32_t PolarityInverter::audioInputGroupCount() const
{
    return 1U;
}

std::uint32_t PolarityInverter::audioOutputGroupCount() const
{
    return 1U;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    PolarityInverter::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0? OptionalAudioChannelGroup(input_): std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    PolarityInverter::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0? OptionalAudioChannelGroup(output_): std::nullopt;
}

std::uint32_t PolarityInverter::latencyInSamples() const
{
    return 0U;
}

template<bool Aligned>
void process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    std::uint64_t invert);

const float sign[2] = {1.0f, -1.0f};

template<>
void process<false>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    std::uint64_t invert)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        auto mul = sign[static_cast<bool>(invert & (1ULL << i))];
        FOR_RANGE0(j, audioProcessData.singleBufferSize)
        {
            audioProcessData.outputs[0][i][j] = std::copysignf(
                audioProcessData.inputs[0][i][j], mul
            );
        }
    }
}

#ifdef _MSC_VER
__m128 mul[2] = {
    __m128 {.m128_f32 = {1.0f, 1.0f, 1.0f, 1.0f}},
    __m128 {.m128_f32 = {-1.0f, -1.0f, -1.0f, -1.0f}},
};
#else
#ifdef __GNUC__
__m128 mul[2] = {
    __m128 {1.0f, 1.0f, 1.0f, 1.0f},
    __m128 {-1.0f, -1.0f, -1.0f, -1.0f}
};
#endif
#endif


template<>
void process<true>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    std::uint64_t invert)
{
    constexpr auto floatCount = sizeof(__m128) / sizeof(float);
    auto alignCount = audioProcessData.singleBufferSize / floatCount;
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        bool inv = static_cast<bool>(invert & (1ULL << i));
        auto input = audioProcessData.inputs[0][i];
        auto alignedInput = reinterpret_cast<__m128*>(input);
        auto output = audioProcessData.outputs[0][i];
        FOR_RANGE0(j, alignCount)
        {
            _mm_store_ps(
                output + j * floatCount,
                _mm_mul_ps(
                    alignedInput[j],
                    mul[inv]
                )
            );
        }
        FOR_RANGE(j, floatCount * alignCount, audioProcessData.singleBufferSize)
        {
            output[j] = input[j] * sign[inv];
        }
    }
}

using ProcessFunc = void(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    std::uint64_t invert);

ProcessFunc* const processFuncs[2] = {
    &process<false>,
    &process<true>
};

void PolarityInverter::process(const Device::AudioProcessData<float>& audioProcessData)
{
    auto invert = invert_.load(std::memory_order_acquire);
    auto aligned = std::all_of(
        audioProcessData.outputs[0] + 0,
        audioProcessData.outputs[0] + audioProcessData.outputCounts[0],
        [](float* buffer)
        {
            return (reinterpret_cast<std::uint64_t>(buffer) & 0x0000000F) == 0;
        }
    ) && std::all_of(
        audioProcessData.inputs[0] + 0,
        audioProcessData.inputs[0] + audioProcessData.inputCounts[0],
        [](float* buffer)
        {
            return (reinterpret_cast<std::uint64_t>(buffer) & 0x0000000F) == 0;
        }
    );
    processFuncs[aligned](audioProcessData, invert);
}

std::uint64_t PolarityInverter::inverted() const
{
    return invert_.load(std::memory_order_acquire);
}

void PolarityInverter::setInverted(std::uint64_t inverted)
{
    invert_.store(inverted, std::memory_order_release);
}
}
