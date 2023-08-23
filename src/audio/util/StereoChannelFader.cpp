#include "StereoChannelFader.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Util
{
using YADAW::Audio::Device::IAudioDevice;

StereoChannelFader::StereoChannelFader()
{
    inputAudioChannelGroup_.setName("StereoChannelFader Main Input");
    inputAudioChannelGroup_.setMain(true);
    inputAudioChannelGroup_.setChannelGroupType(YADAW::Audio::Base::ChannelGroupType::eStereo);
    outputAudioChannelGroup_.setName("StereoChannelFader Main Output");
    outputAudioChannelGroup_.setMain(true);
    outputAudioChannelGroup_.setChannelGroupType(YADAW::Audio::Base::ChannelGroupType::eStereo);
}

StereoChannelFader::~StereoChannelFader()
{}

std::uint32_t StereoChannelFader::audioInputGroupCount() const
{
    return 1;
}

std::uint32_t StereoChannelFader::audioOutputGroupCount() const
{
    return 1;
}

IAudioDevice::OptionalAudioChannelGroup StereoChannelFader::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0?
        IAudioDevice::OptionalAudioChannelGroup(std::reference_wrapper(inputAudioChannelGroup_)):
        std::nullopt;
}

IAudioDevice::OptionalAudioChannelGroup StereoChannelFader::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0?
        IAudioDevice::OptionalAudioChannelGroup(std::reference_wrapper(outputAudioChannelGroup_)):
        std::nullopt;
}

std::uint32_t StereoChannelFader::latencyInSamples() const
{
    return 0;
}

void StereoChannelFader::process(const Device::AudioProcessData<float>& audioProcessData)
{
    using namespace YADAW::Audio::Base;
    auto scale = scaleFromPanning<PanLaw::ConstantPowerSineCompensate>(panning_);
    FOR_RANGE0(i, audioProcessData.singleBufferSize)
    {
        audioProcessData.outputs[0][0][i] = audioProcessData.inputs[0][0][i] * gain_ * scale.left;
    }
    FOR_RANGE0(i, audioProcessData.singleBufferSize)
    {
        audioProcessData.outputs[0][1][i] = audioProcessData.inputs[0][1][i] * gain_ * scale.right;
    }
}

void StereoChannelFader::setGain(float gain)
{
    gain_ = gain;
}

void StereoChannelFader::setPanning(float panning)
{
    panning_ = panning;
}
}