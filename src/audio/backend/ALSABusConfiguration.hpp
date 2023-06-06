#ifndef YADAW_SRC_AUDIO_BACKEND_ALSABUSCONFIGURATION
#define YADAW_SRC_AUDIO_BACKEND_ALSABUSCONFIGURATION

#if(__linux__)

#include "audio/backend/ALSABackend.hpp"
#include "audio/device/IAudioBusConfiguration.hpp"
#include "util/OptionalUtil.hpp"

#include <optional>

namespace YADAW::Audio::Backend
{
using YADAW::Audio::Device::Channel;
using YADAW::Audio::Device::IBus;
class ALSABusConfiguration: public YADAW::Audio::Device::IAudioBusConfiguration
{
public:
    class Bus: public YADAW::Audio::Device::IBus
    {
    public:
        Bus(std::uint32_t channelCount);
    public:
        std::optional<Channel> channelAt(std::uint32_t index) const override;
        bool setChannel(std::uint32_t index, Channel channel) override;
    private:
        std::vector<Channel> channels_;
    };
public:
    ALSABusConfiguration(const ALSABackend& backend);
public:
    std::uint32_t inputBusCount() const override;
    std::uint32_t outputBusCount() const override;
    OptionalRef<const IBus> inputBusAt(std::uint32_t index) const override;
    OptionalRef<const IBus> outputBusAt(std::uint32_t index) const override;
    OptionalRef<IBus> inputBusAt(std::uint32_t index) override;
    OptionalRef<IBus> outputBusAt(std::uint32_t index) override;
    ChannelPosition channelPosition(bool isInput, Channel channel) const override;
    uint32_t appendBus(bool isInput, std::uint32_t channelCount) override;
    bool removeBus(bool isInput, std::uint32_t index) override;
    void clearBus(bool isInput) override;
private:
    const ALSABackend* backend_;
    std::vector<Bus> inputBusses_;
    std::vector<Bus> outputBusses_;
};
}

#endif

#endif //YADAW_SRC_AUDIO_BACKEND_ALSABUSCONFIGURATION
