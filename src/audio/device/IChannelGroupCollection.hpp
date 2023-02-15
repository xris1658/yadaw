#ifndef YADAW_AUDIO_DEVICE_ICHANNELGROUPCOLLECTION
#define YADAW_AUDIO_DEVICE_ICHANNELGROUPCOLLECTION

#include "audio/device/IChannelGroup.hpp"

#include <cstdint>

namespace YADAW::Audio::Device
{
class IChannelGroupCollection
{
public:
    virtual std::uint8_t channelGroupCount() const = 0;
    virtual const IChannelGroup& channelGroupAt(std::uint8_t index) const = 0;
};
}

#endif //YADAW_AUDIO_DEVICE_ICHANNELGROUPCOLLECTION
