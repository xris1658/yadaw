#include "VolumeFader.hpp"

#include "audio/base/Gain.hpp"
#include "audio/host/HostContext.hpp"
#include "util/IntegerRange.hpp"

#include <cstdint>
#include <QCoreApplication>

#include <xmmintrin.h>

#include "util/Util.hpp"

namespace YADAW::Audio::Mixer
{

VolumeFader::GainParameter::GainParameter(VolumeFader& volumeFader):
    volumeFader_(volumeFader)
{
    flags_ = YADAW::Audio::Device::ParameterFlags::SupportDefaultValue
        | YADAW::Audio::Device::ParameterFlags::SupportMinMaxValue
        | YADAW::Audio::Device::ParameterFlags::Automatable;
}

VolumeFader::GainParameter::~GainParameter()
{}

std::uint32_t VolumeFader::GainParameter::id() const
{
    return VolumeFader::ParamId::Gain;
}

QString VolumeFader::GainParameter::name() const
{
    return QCoreApplication::translate("VolumeFader", "Gain");
}

double VolumeFader::GainParameter::minValue() const
{
    return -144.0;
}

double VolumeFader::GainParameter::maxValue() const
{
    return +6.0;
}

double VolumeFader::GainParameter::defaultValue() const
{
    return 0.0;
}

double VolumeFader::GainParameter::value() const
{
    return volumeFader_.prevValue_;
}

double VolumeFader::GainParameter::stepSize() const
{
    return 0;
}

std::uint32_t VolumeFader::GainParameter::stepCount() const
{
    return 0;
}

QString VolumeFader::GainParameter::unit() const
{
    return "dB";
}

QString VolumeFader::GainParameter::valueToString(double value) const
{
    return QString::number(value);
}

double VolumeFader::GainParameter::stringToValue(const QString& string) const
{
    bool ok = false;
    double ret = 0.0;
    ret = string.toDouble(&ok);
    if(ok)
    {
        ret = std::clamp(ret, minValue(), maxValue());
        return ret;
    }
    return NAN;
}

VolumeFader::VolumeFader(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup):
    input_(), output_(), gainParameter_(*this)
{
    input_.setChannelGroupType(channelGroupType, channelCountInGroup);
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
}

bool VolumeFader::initialize(double sampleRate, std::uint32_t maxSampleCount)
{
    if(sampleRate > 0
        && maxSampleCount > 0
        && maxSampleCount < valueSeq_[0].max_size())
    {
        sampleRate_ = sampleRate;
        valueSeq_[0].resize(maxSampleCount, 1.0);
        valueSeq_[1].resize(maxSampleCount, 1.0);
        return true;
    }
    return false;
}

void VolumeFader::uninitialize()
{
    valueSeq_[0].clear();
    valueSeq_[0].shrink_to_fit();
    valueSeq_[1].clear();
    valueSeq_[1].shrink_to_fit();
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

template<bool Aligned>
void process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    YADAW::Audio::Base::Automation::Value* valueSequence);

template<>
void process<false>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    double* valueSequence)
{
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        auto input = audioProcessData.inputs[0][i];
        std::transform(
            input,
            input + audioProcessData.singleBufferSize,
            valueSequence,
            audioProcessData.outputs[0][i],
            [](float lhs, double rhs) { return lhs * rhs; }
        );
    }
}

template<>
void process<true>(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    double* valueSequence)
{
    auto alignedValueSeq = reinterpret_cast<__m128d*>(valueSequence);
    constexpr auto floatCount = sizeof(__m128) / sizeof(float);
    auto alignCount = audioProcessData.singleBufferSize / floatCount;
    FOR_RANGE0(i, audioProcessData.outputCounts[0])
    {
        auto input = audioProcessData.inputs[0][i];
        auto alignedInput = reinterpret_cast<__m128*>(input);
        auto output = audioProcessData.outputs[0][i];
        FOR_RANGE0(j, alignCount)
        {
            _mm_store_ps(
                output + j * floatCount,
                _mm_mul_ps(
                    alignedInput[j],
                    _mm_movelh_ps(                                // | 0.0 | 1.0 | 2.0 | 3.0 |
                                                                  //    ^     ^
                                                                  //    |     |___________
                                                                  //    |___________      |
                                                                  //                |     |
                        _mm_cvtpd_ps(alignedValueSeq[j * 2    ]), // |     |     | 0.0 | 1.0 |
                        _mm_cvtpd_ps(alignedValueSeq[j * 2 + 1])  // |     |     | 2.0 | 3.0 |
                    )
                )
            );
        }
        std::transform(
            input + floatCount * alignCount,
            input + audioProcessData.singleBufferSize,
            valueSequence + floatCount * alignCount,
            audioProcessData.outputs[0][i] + floatCount * alignCount,
            [](float lhs, double rhs) { return lhs * rhs; }
        );
    }
}

using ProcessFunc = void(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
    YADAW::Audio::Base::Automation::Value* valueSequence);

ProcessFunc* const processFuncs[2] = {
    &process<false>,
    &process<true>
};

void VolumeFader::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    auto currentPluginIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    auto valueSeq = valueSeq_[currentPluginIndex];
    auto valuePoints = valuePoints_[currentPluginIndex];
    auto valuePointCount = valuePoints.size();
    if(valuePointCount > 0)
    {
        double value = YADAW::Audio::Base::scaleFromDecibel(valuePoints.front().second);
        std::fill(
            valueSeq.begin(), valueSeq.begin() + valuePoints.front().first,
            value
        );
        for(decltype(valuePointCount) i = 0; i < valuePointCount - 1; ++i)
        {
            auto delta = YADAW::Audio::Base::scaleFromDecibel(
                (valuePoints[i + 1].second - valuePoints[i].second) / (valuePoints[i + 1].first - valuePoints[i].first)
            );
            FOR_RANGE(j, valuePoints[i].first, valuePoints[i + 1].first)
            {
                valueSeq[j] = value;
                value *= delta;
            }
        }
        value = YADAW::Audio::Base::scaleFromDecibel(valuePoints.back().second);
        std::fill(valueSeq.begin() + valuePoints.back().first, valueSeq.end(), value);
    }
    else
    {
        std::fill(
            valueSeq.begin(), valueSeq.end(),
            YADAW::Audio::Base::scaleFromDecibel(prevValue_)
        );
    }
    bool index2 =
        std::all_of(
            audioProcessData.outputs[0],
            audioProcessData.outputs[0] + audioProcessData.outputCounts[0],
            [](float* buffer)
            {
                return (reinterpret_cast<std::uint64_t>(buffer) & 0x0000000F) == 0;
            }
        ) &&
        std::all_of(
            audioProcessData.inputs[0],
            audioProcessData.inputs[0] + audioProcessData.inputCounts[0],
            [](float* buffer)
            {
                return (reinterpret_cast<std::uint64_t>(buffer) & 0x0000000F) == 0;
            }
        )
     && (reinterpret_cast<std::uint64_t>(valueSeq.data()) & 0x0000000F) == 0;
    processFuncs[index2](audioProcessData, valueSeq.data());
}

std::uint32_t VolumeFader::parameterCount() const
{
    return 1U;
}

const YADAW::Audio::Device::IParameter* VolumeFader::parameter(std::uint32_t index) const
{
    return index == 0? &gainParameter_: nullptr;
}

YADAW::Audio::Device::IParameter* VolumeFader::parameter(std::uint32_t index)
{
    return const_cast<YADAW::Audio::Device::IParameter*>(
        static_cast<const VolumeFader*>(this)->parameter(index)
    );
}

void VolumeFader::beginEditGain()
{
    addPoint();
}

void VolumeFader::performEditGain(double value)
{
    addPoint(value);
}

void VolumeFader::endEditGain()
{
    addPoint();
}

void VolumeFader::onBufferSwitched(std::int64_t switchTimestampInNanosecond)
{
    auto currentPluginBufferIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    switchTimestampInNanosecond_ = switchTimestampInNanosecond;
    auto currentHostBufferIndex = currentPluginBufferIndex ^ 1;
    valuePoints_[currentHostBufferIndex].clear();
    auto& pluginValuePoints = valuePoints_[currentPluginBufferIndex];
    if(!pluginValuePoints.empty())
    {
        prevValue_ = pluginValuePoints.back().second;
    }
}

void VolumeFader::addPoint()
{
    auto hostIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get() ^ 1;
    std::uint32_t sampleOffset = std::round(
        (YADAW::Util::currentTimeValueInNanosecond() - switchTimestampInNanosecond_) / (sampleRate_ * 1000000000)
    );
    auto& valuePoints = valuePoints_[hostIndex];
    if(valuePoints.size() == 0)
    {
        valuePoints.emplace_back(sampleOffset, prevValue_);
    }
    else
    {
        if(auto& back = valuePoints.back(); back.first != sampleOffset)
        {
            valuePoints.emplace_back(sampleOffset, back.second);
        }
    }
}

void VolumeFader::addPoint(double value)
{
    auto hostIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get() ^ 1;
    std::uint32_t sampleOffset = std::round(
        (YADAW::Util::currentTimeValueInNanosecond() - switchTimestampInNanosecond_) / (sampleRate_ * 1000000000)
    );
    auto& valuePoints = valuePoints_[hostIndex];
    if(valuePoints.size() == 0)
    {
        valuePoints.emplace_back(sampleOffset, prevValue_);
    }
    else
    {
        if(auto& back = valuePoints.back(); back.first != sampleOffset)
        {
            valuePoints.emplace_back(sampleOffset, back.second);
        }
        else
        {
            back.second = value;
        }
    }
}
}
