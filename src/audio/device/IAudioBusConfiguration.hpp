#ifndef YADAW_SRC_AUDIO_DEVICE_IAUDIOBUSCONFIGURATION
#define YADAW_SRC_AUDIO_DEVICE_IAUDIOBUSCONFIGURATION

#include "audio/base/Channel.hpp"
#include "audio/device/IAudioChannelGroup.hpp"
#include "audio/device/IAudioDevice.hpp"
#include "util/OptionalUtil.hpp"

#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

namespace YADAW::Audio::Device
{
static constexpr auto InvalidIndex = std::numeric_limits<std::uint32_t>::max();
struct Channel
{
    std::uint32_t deviceIndex = InvalidIndex;
    std::uint32_t channelIndex = InvalidIndex;
    bool operator==(Channel rhs) const
    {
        return reinterpret_cast<const std::uint64_t&>(deviceIndex) == reinterpret_cast<const std::uint64_t&>(rhs.deviceIndex);
    }
    bool operator!=(Channel rhs) const
    {
        return !operator==(rhs);
    }
};

class IBus: public YADAW::Audio::Device::IAudioDevice
{
public:
    IBus(YADAW::Audio::Base::ChannelGroupType channelGroupType, std::uint32_t channelCount = 0):
        channelGroupType_(channelGroupType),
        channelCount_(0)
    {
        if(channelCount == 0)
        {
            auto actualChannelCount = YADAW::Audio::Device::IAudioChannelGroup::channelCount(channelGroupType_);
            if(actualChannelCount == YADAW::Audio::Base::InvalidChannelCount)
            {
                throw std::invalid_argument("The channel count of the specified group type is unknown");
            }
            channelCount = actualChannelCount;
        }
        channelCount_ = channelCount;
    }
    IBus(std::uint32_t channelCount): channelCount_(channelCount) {}
    virtual ~IBus() = default;
public:
    YADAW::Audio::Base::ChannelGroupType channelGroupType() const
    {
        return channelGroupType_;
    }
    std::uint32_t channelCount() const { return channelCount_; }
    virtual std::optional<Channel> channelAt(std::uint32_t index) const = 0;
    virtual bool setChannel(std::uint32_t index, Channel channel) = 0;
protected:
    YADAW::Audio::Base::ChannelGroupType channelGroupType_;
    std::uint32_t channelCount_;
};

class IAudioBusConfiguration
{
public:
    struct ChannelPosition
    {
        std::uint32_t busIndex = InvalidIndex;
        std::uint32_t channelIndex = InvalidIndex;
    };
public:
    virtual ~IAudioBusConfiguration() {}
public:
    virtual std::uint32_t inputBusCount() const = 0;
    virtual std::uint32_t outputBusCount() const = 0;
    virtual OptionalRef<const IBus> inputBusAt(std::uint32_t index) const = 0;
    virtual OptionalRef<const IBus> outputBusAt(std::uint32_t index) const = 0;
    virtual OptionalRef<IBus> inputBusAt(std::uint32_t index) = 0;
    virtual OptionalRef<IBus> outputBusAt(std::uint32_t index) = 0;
    virtual ChannelPosition channelPosition(bool isInput, Channel channel) const = 0;
    virtual std::uint32_t appendBus(bool isInput,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCount = 0) = 0;
    virtual bool insertBus(
        std::uint32_t position,
        bool isInput,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCount = 0) = 0;
    virtual bool removeBus(bool isInput, std::uint32_t index) = 0;
    virtual void clearBus(bool isInput) = 0;
};
}

#endif // YADAW_SRC_AUDIO_DEVICE_IAUDIOBUSCONFIGURATION
