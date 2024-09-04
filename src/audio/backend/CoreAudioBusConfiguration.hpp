#ifndef YADAW_SRC_AUDIO_BACKEND_COREAUDIOBUSCONFIGURATION
#define YADAW_SRC_AUDIO_BACKEND_COREAUDIOBUSCONFIGURATION

#if __APPLE__

#include "audio/backend/CoreAudioBackend.hpp"
#include "audio/base/Channel.hpp"
#include "audio/device/IAudioBusConfiguration.hpp"
#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"
#include "util/OptionalUtil.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace YADAW::Audio::Backend
{
using YADAW::Audio::Device::Channel;
using YADAW::Audio::Device::IBus;
class CoreAudioBusConfiguration: public YADAW::Audio::Device::IAudioBusConfiguration
{
public:
    CoreAudioBusConfiguration();
public:
    std::uint32_t inputBusCount() const override;
    std::uint32_t outputBusCount() const override;
    OptionalRef<const IBus> inputBusAt(std::uint32_t index) const override;
    OptionalRef<const IBus> outputBusAt(std::uint32_t index) const override;
    OptionalRef<IBus> inputBusAt(std::uint32_t index) override;
    OptionalRef<IBus> outputBusAt(std::uint32_t index) override;
    ChannelPosition channelPosition(bool isInput, Channel channel) const override;
    std::uint32_t appendBus(bool isInput,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCount = 0) override;
    bool insertBus(
        std::uint32_t position,
        bool isInput,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCount) override;
    bool removeBus(bool isInput, std::uint32_t index) override;
    void clearBus(bool isInput) override;
public:
    OptionalRef<const IBus> getInputBusAt(std::uint32_t index) const;
    OptionalRef<const IBus> getOutputBusAt(std::uint32_t index) const;
    OptionalRef<IBus> getInputBusAt(std::uint32_t index);
    OptionalRef<IBus> getOutputBusAt(std::uint32_t index);
};
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_COREAUDIOBUSCONFIGURATION