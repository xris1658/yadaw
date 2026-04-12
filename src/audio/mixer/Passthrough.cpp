#include "Passthrough.hpp"

namespace YADAW::Audio::Mixer
{
Passthrough::Passthrough(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup):
    input_(), output_()
{
    input_.setChannelGroupType(channelGroupType, channelCountInGroup);
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
}

std::uint32_t Passthrough::audioInputGroupCount() const
{
    return 1;
}

std::uint32_t Passthrough::audioOutputGroupCount() const
{
    return 1;
}

Device::IAudioDevice::OptionalAudioChannelGroup Passthrough::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(std::ref(input_)):
        std::nullopt;
}

Device::IAudioDevice::OptionalAudioChannelGroup Passthrough::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(std::ref(output_)):
        std::nullopt;
}

std::uint32_t Passthrough::latencyInSamples() const
{
    return 0;
}

void Passthrough::process(
    const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    for(int i = 0; i < audioProcessData.outputGroupCount; ++i)
    {
        for(int j = 0; j < audioProcessData.inputCounts[i]; ++j)
        {
            std::memcpy(
                audioProcessData.outputs[i][j],
                audioProcessData.inputs[i][j],
                audioProcessData.singleBufferSize * sizeof(float) * (audioProcessData.outputs[i][j] != audioProcessData.inputs[i][j])
            );
        }
    }
}
}
