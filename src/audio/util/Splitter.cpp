#include "Splitter.hpp"

namespace YADAW::Audio::Util
{
Splitter::Splitter(std::uint32_t outputCount, YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    outputs_.reserve(outputCount);
    for(decltype(outputCount) i = 0; i < outputCount; ++i)
    {
        outputs_.emplace_back().setChannelGroupType(channelGroupType, channelCountInGroup);
    }
    input_.setChannelGroupType(channelGroupType, channelCountInGroup);
    input_.setMain(true);
}

std::uint32_t Splitter::audioInputGroupCount() const
{
    return 1;
}

std::uint32_t Splitter::audioOutputGroupCount() const
{
    return outputs_.size();
}

Device::IAudioDevice::OptionalAudioChannelGroup Splitter::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0? OptionalAudioChannelGroup(std::ref(input_)): std::nullopt;
}

Device::IAudioDevice::OptionalAudioChannelGroup Splitter::audioOutputGroupAt(std::uint32_t index) const
{
    return index < audioOutputGroupCount()? OptionalAudioChannelGroup(std::ref(outputs_[index])): std::nullopt;
}

std::uint32_t Splitter::latencyInSamples() const
{
    return 0;
}

void Splitter::process(const Device::AudioProcessData<float>& audioProcessData)
{
    auto outputCount = outputs_.size();
    for(std::uint32_t i = 0; i < outputCount; ++i)
    {
        for(std::uint32_t j = 0; j < input_.channelCount(); ++j)
        {
            std::memmove(
                audioProcessData.outputs[i][j], audioProcessData.inputs[0][j],
                audioProcessData.singleBufferSize * sizeof(float));
        }
    }
}
}