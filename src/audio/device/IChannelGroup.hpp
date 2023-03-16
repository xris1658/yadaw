#ifndef YADAW_AUDIO_DEVICE_ICHANNELGROUP
#define YADAW_AUDIO_DEVICE_ICHANNELGROUP

#include "audio/base/Channel.hpp"

#include <QString>

namespace YADAW::Audio::Device
{
class IChannelGroup
{
public:
    virtual ~IChannelGroup() = default;
public:
    virtual QString name() const = 0;
    virtual std::uint8_t channelCount() const = 0;
    virtual YADAW::Audio::Base::ChannelGroupType type() const = 0;
    virtual YADAW::Audio::Base::ChannelType speakerAt(std::uint8_t index) const = 0;
    virtual QString speakerNameAt(std::uint8_t index) const = 0;
    virtual bool isMain() const = 0;
protected:
    static int channelCount(YADAW::Audio::Base::ChannelGroupType channelGroupType);
    static bool isChannelInGroup(YADAW::Audio::Base::ChannelType channelType, YADAW::Audio::Base::ChannelGroupType channelGroupType);
};
}

#endif //YADAW_AUDIO_DEVICE_ICHANNELGROUP
