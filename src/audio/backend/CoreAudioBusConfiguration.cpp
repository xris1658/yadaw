#if __APPLE__

#include "CoreAudioBusConfiguration.hpp"

namespace YADAW::Audio::Backend
{
CoreAudioBusConfiguration::CoreAudioBusConfiguration()
{
}

std::uint32_t CoreAudioBusConfiguration::inputBusCount() const
{
    return 0;
}

std::uint32_t CoreAudioBusConfiguration::outputBusCount() const
{
    return 0;
}

OptionalRef<const IBus> CoreAudioBusConfiguration::inputBusAt(std::uint32_t index) const
{
    return std::nullopt;
}

OptionalRef<const IBus> CoreAudioBusConfiguration::outputBusAt(std::uint32_t index) const
{
    return std::nullopt;
}

OptionalRef<IBus> CoreAudioBusConfiguration::inputBusAt(std::uint32_t index)
{
    return std::nullopt;
}

OptionalRef<IBus> CoreAudioBusConfiguration::outputBusAt(std::uint32_t index)
{
    return std::nullopt;
}

Device::IAudioBusConfiguration::ChannelPosition CoreAudioBusConfiguration::channelPosition(bool isInput,
    Channel channel) const
{
    return IAudioBusConfiguration::ChannelPosition {0U, 0U};
}

std::uint32_t CoreAudioBusConfiguration::appendBus(bool isInput, YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCount)
{
    return -1;
}

bool CoreAudioBusConfiguration::insertBus(std::uint32_t position, bool isInput,
    YADAW::Audio::Base::ChannelGroupType channelGroupType, std::uint32_t channelCount)
{
    return false;
}

bool CoreAudioBusConfiguration::removeBus(bool isInput, std::uint32_t index)
{
    return false;
}

void CoreAudioBusConfiguration::clearBus(bool isInput)
{}

OptionalRef<const IBus> CoreAudioBusConfiguration::getInputBusAt(std::uint32_t index) const
{
    return std::nullopt;
}

OptionalRef<const IBus> CoreAudioBusConfiguration::getOutputBusAt(std::uint32_t index) const
{
    return std::nullopt;
}

OptionalRef<IBus> CoreAudioBusConfiguration::getInputBusAt(std::uint32_t index)
{
    return std::nullopt;
}

OptionalRef<IBus> CoreAudioBusConfiguration::getOutputBusAt(std::uint32_t index)
{
    return std::nullopt;
}
}

#endif