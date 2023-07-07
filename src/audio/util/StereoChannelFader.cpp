#include "StereoChannelFader.hpp"

namespace YADAW::Audio::Util
{
using YADAW::Audio::Device::IAudioDevice;

constexpr YADAW::Audio::Base::ChannelGroupType StereoChannelFader::AudioChannelGroup::channelGroupType()
{
    return Base::ChannelGroupType::Stereo;
}

QString StereoChannelFader::AudioChannelGroup::name() const
{
    return name_;
}

std::uint32_t StereoChannelFader::AudioChannelGroup::channelCount() const
{
    return YADAW::Audio::Device::IAudioChannelGroup::channelCount(channelGroupType());
}

YADAW::Audio::Base::ChannelGroupType StereoChannelFader::AudioChannelGroup::type() const
{
    return channelGroupType();
}

YADAW::Audio::Base::ChannelType StereoChannelFader::AudioChannelGroup::speakerAt(std::uint32_t index) const
{
    return YADAW::Audio::Device::IAudioChannelGroup::channelAt(
        channelGroupType(), index);
}

QString StereoChannelFader::AudioChannelGroup::speakerNameAt(std::uint32_t index) const
{
    return QString();
}

bool StereoChannelFader::AudioChannelGroup::isMain() const
{
    return true;
}

void StereoChannelFader::AudioChannelGroup::setName(const QString& name)
{
    name_ = name;
}

void StereoChannelFader::AudioChannelGroup::setName(QString&& name)
{
    name_ = std::move(name);
}

StereoChannelFader::StereoChannelFader()
{
    inputAudioChannelGroup_.setName("StereoChannelFader Main Input");
    outputAudioChannelGroup_.setName("StereoChannelFader Main Output");
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
    for(decltype(audioProcessData.singleBufferSize) i = 0; i < audioProcessData.singleBufferSize; ++i)
    {
        audioProcessData.outputs[0][0][i] = audioProcessData.inputs[0][0][i] * gain_ * scale.left;
    }
    for(decltype(audioProcessData.singleBufferSize) i = 0; i < audioProcessData.singleBufferSize; ++i)
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