#include "VolumeFader.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Mixer
{
VolumeFader::VolumeFader(
    const YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup):
    input_(), output_()
{
    input_.setChannelGroupType(channelGroupType, channelCountInGroup);
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
}

float VolumeFader::scale() const
{
    return scale_;
}

void VolumeFader::setScale(float scale)
{
    scale_ = scale;
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




void VolumeFader::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    (this->*processFuncs[automation_ != nullptr])(audioProcessData);
}

void VolumeFader::processWithoutAutomation(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    // slower but necessary traversing
    FOR_RANGE0(j, audioProcessData.singleBufferSize)
    {
        FOR_RANGE0(i, output_.channelCount())
        {
            audioProcessData.outputs[0][i][j] =
                audioProcessData.inputs[0][i][j] * scale_;
        }
    }
}

void VolumeFader::processWithAutomation(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    // slower but necessary traversing
    FOR_RANGE0(j, audioProcessData.singleBufferSize)
    {
        FOR_RANGE0(i, output_.channelCount())
        {
            audioProcessData.outputs[0][i][j] =
                audioProcessData.inputs[0][i][j] * (*automation_)(time_ + j);
        }
    }
}

void VolumeFader::setAutomation(
    const YADAW::Audio::Base::Automation& automation,
    YADAW::Audio::Base::Automation::Time time)
{
    automation_ = &automation;
    time_ = time;
}

void VolumeFader::unsetAutomation(const Base::Automation& automation)
{
    auomation_ = nullptr;
}
}