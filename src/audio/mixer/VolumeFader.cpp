#include "VolumeFader.hpp"

#include "util/IntegerRange.hpp"

#include <cstdint>

#include <xmmintrin.h>

namespace YADAW::Audio::Mixer
{
VolumeFader::VolumeFader(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup):
    input_(), output_()
{
    input_.setChannelGroupType(channelGroupType, channelCountInGroup);
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
}

std::uint32_t VolumeFader::audioInputGroupCount() const
{
    return 1;
}

std::uint32_t VolumeFader::audioOutputGroupCount() const
{
    return 1;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    VolumeFader::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(std::ref(input_)):
        std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    VolumeFader::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(std::ref(output_)):
        std::nullopt;
}

std::uint32_t VolumeFader::latencyInSamples() const
{
    return 0;
}

template<bool HasValueSeq, bool Aligned>
void process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    YADAW::Audio::Base::Automation::Value* valueSequence);

template<>
void process<false, false>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    YADAW::Audio::Base::Automation::Value*)
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

template<>
void process<false, true>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    YADAW::Audio::Base::Automation::Value*)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        auto alignCount = audioProcessData.singleBufferSize * sizeof(float) / sizeof(__m128);
        auto alignedInput = reinterpret_cast<__m128*>(audioProcessData.inputs[0][i]);
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

template<>
void process<true, false>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    YADAW::Audio::Base::Automation::Value* valueSequence)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        auto input = audioProcessData.inputs[0][i];
        std::transform(
            input,
            input + audioProcessData.singleBufferSize,
            valueSequence,
            audioProcessData.outputs[0][i],
            [](float lhs, float rhs) { return lhs * rhs; }
        );
    }
}

template<>
void process<true, true>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    YADAW::Audio::Base::Automation::Value* valueSequence)
{
    auto alignedValueSeq = reinterpret_cast<__m128d*>(valueSequence);
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        auto alignCount = audioProcessData.singleBufferSize * sizeof(float) / sizeof(__m128);
        auto alignedInput = reinterpret_cast<__m128*>(audioProcessData.inputs[0][i]);
        auto output = audioProcessData.outputs[0][i];
        FOR_RANGE0(j, alignCount)
        {
            _mm_store_ps(
                output + j * sizeof(__m128) / sizeof(float),
                _mm_mul_ps(
                    alignedInput[j],
                    _mm_movelh_ps(                                // | 0.0 | 1.0 | 2.0 | 3.0 |
                                                                  //    A     A
                                                                  //    |     |___________
                                                                  //    |___________      |
                                                                  //                |     |
                        _mm_cvtpd_ps(alignedValueSeq[j * 2    ]), // |     |     | 0.0 | 1.0 |
                        _mm_cvtpd_ps(alignedValueSeq[j * 2 + 1])  // |     |     | 2.0 | 3.0 |
                    )
                )
            );
        }
    }
}

using ProcessFunc = void(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    YADAW::Audio::Base::Automation::Value* valueSequence);

ProcessFunc* const processFuncs[2][2] = {
    {
        &process<false, false>,
        &process<false, true>,
    },
    {
        &process<true, false>,
        &process<true, true>
    }
};

void VolumeFader::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    bool index1 = valueSequence_ != nullptr;
    bool index2 =
        (reinterpret_cast<std::uint64_t>(audioProcessData.outputs[0][0]) & 0x0000000F) == 0
     && (reinterpret_cast<std::uint64_t>(audioProcessData.inputs [0][0]) & 0x0000000F) == 0
     && (reinterpret_cast<std::uint64_t>(valueSequence_)                 & 0x0000000F) == 0;
    processFuncs[index1][index2](audioProcessData, valueSequence_);
}

void VolumeFader::setVolumeValueSequence(YADAW::Audio::Base::Automation::Value& value)
{
    valueSequence_ = &value;
}

void VolumeFader::unsetVolumeValueSequence()
{
    valueSequence_ = nullptr;
}
}
