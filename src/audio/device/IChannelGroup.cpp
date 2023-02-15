#include "IChannelGroup.hpp"

namespace YADAW::Audio::Device
{
int IChannelGroup::channelCount(YADAW::Audio::Base::ChannelGroupType channelGroupType)
{
    switch(channelGroupType)
    {
    case YADAW::Audio::Base::ChannelGroupType::NoChannel:
        return 0;
    case YADAW::Audio::Base::ChannelGroupType::Mono:
        return 1;
    case YADAW::Audio::Base::ChannelGroupType::Stereo:
        return 2;
    default:
        return YADAW::Audio::Base::InvalidChannelCount;
    }
}

bool IChannelGroup::isChannelInGroup(YADAW::Audio::Base::ChannelType channelType, YADAW::Audio::Base::ChannelGroupType channelGroupType)
{
    switch(channelGroupType)
    {
    case YADAW::Audio::Base::ChannelGroupType::Mono:
        return channelType == YADAW::Audio::Base::ChannelType::Center;
    case YADAW::Audio::Base::ChannelGroupType::Stereo:
        return channelType == YADAW::Audio::Base::ChannelType::Left
               || channelType == YADAW::Audio::Base::ChannelType::Right;
    default:
        return false;
    }
}
}