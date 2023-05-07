#include "AudioGraphBusConfiguration.hpp"

namespace YADAW::Audio::Backend
{
using YADAW::Audio::Device::Channel;
using YADAW::Audio::Device::IBus;
using ChannelPosition = YADAW::Audio::Device::IAudioBusConfiguration::ChannelPosition;

AudioGraphBusConfiguration::Bus::Bus(std::uint32_t channelCount):
    IBus(channelCount),
    channels_(channelCount)
{}

std::optional<Channel> AudioGraphBusConfiguration::Bus::channelAt(std::uint32_t index) const
{
    return index < channelCount_?
        std::optional<Channel>(channels_[index]):
        std::nullopt;
}

bool AudioGraphBusConfiguration::Bus::setChannel(std::uint32_t index, Channel channel)
{
    if(index < channels_.size())
    {
        channels_[index] = channel;
        return true;
    }
    return false;
}

AudioGraphBusConfiguration::AudioGraphBusConfiguration(const AudioGraphBackend& backend):
    backend_(&backend),
    inputBusses_(),
    outputBusses_()
{
    //
}

std::uint32_t AudioGraphBusConfiguration::inputBusCount() const
{
    return inputBusses_.size();
}

std::uint32_t AudioGraphBusConfiguration::outputBusCount() const
{
    return outputBusses_.size();
}

OptionalRef<const IBus> AudioGraphBusConfiguration::inputBusAt(std::uint32_t index) const
{
    return index < inputBusCount()? OptionalRef<const IBus>(inputBusses_[index]): std::nullopt;
}

OptionalRef<const IBus> AudioGraphBusConfiguration::outputBusAt(std::uint32_t index) const
{
    return index < outputBusCount()? OptionalRef<const IBus>(outputBusses_[index]): std::nullopt;
}

OptionalRef<IBus> AudioGraphBusConfiguration::inputBusAt(std::uint32_t index)
{
    return index < inputBusCount()? OptionalRef<IBus>(inputBusses_[index]): std::nullopt;
}

OptionalRef<IBus> AudioGraphBusConfiguration::outputBusAt(std::uint32_t index)
{
    return index < outputBusCount()? OptionalRef<IBus>(outputBusses_[index]): std::nullopt;
}

ChannelPosition AudioGraphBusConfiguration::channelPosition(bool isInput, Channel channel) const
{
    const auto& busses = isInput? inputBusses_: outputBusses_;
    for(std::uint32_t i = 0; i < busses.size(); ++i)
    {
        const auto& bus = busses[i];
        for(std::uint32_t j = 0; j < bus.channelCount(); ++j)
        {
            auto channelInBus = bus.channelAt(j);
            if(channelInBus == channel)
            {
                return {i, j};
            }
        }
    }
    return {};
}

uint32_t AudioGraphBusConfiguration::appendBus(bool isInput, std::uint32_t channelCount)
{
    auto& bus = isInput? inputBusses_: outputBusses_;
    bus.emplace_back(channelCount);
    return bus.size() - 1;
}

bool AudioGraphBusConfiguration::removeBus(bool isInput, std::uint32_t index)
{
    auto& busses = isInput? inputBusses_: outputBusses_;
    if(index < busses.size())
    {
        busses.erase(busses.begin() + index);
        return true;
    }
    return false;
}

void AudioGraphBusConfiguration::clearBus(bool isInput)
{
    auto& busses = isInput? inputBusses_: outputBusses_;
    busses.clear();
}
}