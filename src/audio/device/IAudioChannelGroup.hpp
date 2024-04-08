#ifndef YADAW_SRC_AUDIO_DEVICE_ICHANNELGROUP
#define YADAW_SRC_AUDIO_DEVICE_ICHANNELGROUP

#include "audio/base/Channel.hpp"

#include <QString>

namespace YADAW::Audio::Device
{
class IAudioChannelGroup
{
public:
    virtual ~IAudioChannelGroup() = default;
public:
    virtual QString name() const = 0;
    virtual std::uint32_t channelCount() const = 0;
    virtual YADAW::Audio::Base::ChannelGroupType type() const = 0;
    virtual YADAW::Audio::Base::ChannelType speakerAt(std::uint32_t index) const = 0;
    virtual QString speakerNameAt(std::uint32_t index) const = 0;
    virtual bool isMain() const = 0;
public:
    static int channelCount(YADAW::Audio::Base::ChannelGroupType channelGroupType);
    static YADAW::Audio::Base::ChannelType channelAt(YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t index);
};
}

#endif // YADAW_SRC_AUDIO_DEVICE_ICHANNELGROUP
