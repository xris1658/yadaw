#include "IAudioChannelGroup.hpp"

using namespace YADAW::Audio::Base;

template<ChannelGroupType>
ChannelType speakers[];

template<>
ChannelType speakers
    <ChannelGroupType::Mono>[]
    = {ChannelType::Center};

template<>
ChannelType speakers
    <ChannelGroupType::Stereo>[]
    = {ChannelType::Left, ChannelType::Right};

namespace YADAW::Audio::Device
{
int IAudioChannelGroup::channelCount(ChannelGroupType channelGroupType)
{
    switch(channelGroupType)
    {
    case ChannelGroupType::NoChannel:
        return 0;
    case ChannelGroupType::Mono:
        return 1;
    case ChannelGroupType::Stereo:
        return 2;
    default:
        return InvalidChannelCount;
    }
}

ChannelType IAudioChannelGroup::channelAt(
    ChannelGroupType channelGroupType, std::uint32_t index)
{
    switch(channelGroupType)
    {
    case ChannelGroupType::Mono:
        return index == 0? ChannelType::Center:
            ChannelType::Invalid;
    case ChannelGroupType::Stereo:
        return index < 2? speakers<ChannelGroupType::Stereo>[index]:
            ChannelType::Invalid;
    default:
        return ChannelType::Invalid;
    }
}

bool IAudioChannelGroup::isChannelInGroup(ChannelType channelType, ChannelGroupType channelGroupType)
{
    switch(channelGroupType)
    {
    case ChannelGroupType::Mono:
        return channelType == ChannelType::Center;
    case ChannelGroupType::Stereo:
        return channelType == ChannelType::Left
            || channelType == ChannelType::Right;
    default:
        return false;
    }
}
}