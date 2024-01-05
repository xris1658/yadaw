#include "IAudioChannelGroup.hpp"

#include <iterator>

using namespace YADAW::Audio::Base;

template<ChannelGroupType>
ChannelType speakers[] = {};

template<>
ChannelType speakers<ChannelGroupType::eMono>[] = {
    ChannelType::Center
};

template<>
ChannelType speakers<ChannelGroupType::eStereo>[] = {
    ChannelType::Left, ChannelType::Right
};

// https://www.rfc-editor.org/rfc/rfc7845#section-5.1.1.2

template<>
ChannelType speakers<ChannelGroupType::eLRC>[] = {
    ChannelType::Left, ChannelType::FrontCenter, ChannelType::Right
};

template<>
ChannelType speakers<ChannelGroupType::eQuad>[] = {
    ChannelType::Left, ChannelType::Right,
    ChannelType::RearLeft, ChannelType::RearRight
};

template<>
ChannelType speakers<ChannelGroupType::e50>[] = {
    ChannelType::Left, ChannelType::Center, ChannelType::Right,
    ChannelType::RearLeft, ChannelType::RearRight
};

template<>
ChannelType speakers<ChannelGroupType::e51>[] = {
    ChannelType::Left, ChannelType::Center, ChannelType::Right,
    ChannelType::RearLeft, ChannelType::RearRight,
    ChannelType::LFE
};

template<>
ChannelType speakers<ChannelGroupType::e61>[] = {
    ChannelType::Left, ChannelType::Center, ChannelType::Right,
    ChannelType::SideLeft, ChannelType::SideRight,
    ChannelType::RearCenter,
    ChannelType::LFE
};

template<>
ChannelType speakers<ChannelGroupType::e71>[] = {
    ChannelType::Left, ChannelType::Center, ChannelType::Right,
    ChannelType::SideLeft, ChannelType::SideRight,
    ChannelType::RearLeft, ChannelType::RearRight,
    ChannelType::LFE
};

template<ChannelGroupType C>
ChannelType getChannelFromGroup(std::uint32_t index)
{
    return index < std::size(speakers<C>)?
        speakers<C>[index]:
            ChannelType::Invalid;
}

template<ChannelGroupType C>
bool channelIsInGroup(ChannelType channel)
{
    for(auto speaker: speakers<C>)
    {
        if(speaker == channel)
        {
            return true;
        }
    }
    return false;
}

namespace YADAW::Audio::Device
{
int IAudioChannelGroup::channelCount(ChannelGroupType channelGroupType)
{
    if(constexpr auto C = ChannelGroupType::eMono; channelGroupType == C)
    {
        return std::size(speakers<C>);
    }
    if(constexpr auto C = ChannelGroupType::eStereo; channelGroupType == C)
    {
        return std::size(speakers<C>);
    }
    if(constexpr auto C = ChannelGroupType::eLRC; channelGroupType == C)
    {
        return std::size(speakers<C>);
    }
    if(constexpr auto C = ChannelGroupType::eQuad; channelGroupType == C)
    {
        return std::size(speakers<C>);
    }
    if(constexpr auto C = ChannelGroupType::e50; channelGroupType == C)
    {
        return std::size(speakers<C>);
    }
    if(constexpr auto C = ChannelGroupType::e51; channelGroupType == C)
    {
        return std::size(speakers<C>);
    }
    if(constexpr auto C = ChannelGroupType::e61; channelGroupType == C)
    {
        return std::size(speakers<C>);
    }
    if(constexpr auto C = ChannelGroupType::e71; channelGroupType == C)
    {
        return std::size(speakers<C>);
    }
    return InvalidChannelCount;
}

ChannelType IAudioChannelGroup::channelAt(
    ChannelGroupType channelGroupType, std::uint32_t index)
{
    if(constexpr auto C = ChannelGroupType::eMono; channelGroupType == C)
    {
        return getChannelFromGroup<C>(index);
    }
    if(constexpr auto C = ChannelGroupType::eStereo; channelGroupType == C)
    {
        return getChannelFromGroup<C>(index);
    }
    if(constexpr auto C = ChannelGroupType::eLRC; channelGroupType == C)
    {
        return getChannelFromGroup<C>(index);
    }
    if(constexpr auto C = ChannelGroupType::eQuad; channelGroupType == C)
    {
        return getChannelFromGroup<C>(index);
    }
    if(constexpr auto C = ChannelGroupType::e50; channelGroupType == C)
    {
        return getChannelFromGroup<C>(index);
    }
    if(constexpr auto C = ChannelGroupType::e51; channelGroupType == C)
    {
        return getChannelFromGroup<C>(index);
    }
    if(constexpr auto C = ChannelGroupType::e61; channelGroupType == C)
    {
        return getChannelFromGroup<C>(index);
    }
    if(constexpr auto C = ChannelGroupType::e71; channelGroupType == C)
    {
        return getChannelFromGroup<C>(index);
    }
    return ChannelType::Invalid;
}

bool IAudioChannelGroupchannelIsInGroup(ChannelType channelType, ChannelGroupType channelGroupType)
{
    if(constexpr auto C = ChannelGroupType::eMono; channelGroupType == C)
    {
        return channelIsInGroup<C>(channelType);
    }
    if(constexpr auto C = ChannelGroupType::eStereo; channelGroupType == C)
    {
        return channelIsInGroup<C>(channelType);
    }
    if(constexpr auto C = ChannelGroupType::eLRC; channelGroupType == C)
    {
        return channelIsInGroup<C>(channelType);
    }
    if(constexpr auto C = ChannelGroupType::eQuad; channelGroupType == C)
    {
        return channelIsInGroup<C>(channelType);
    }
    if(constexpr auto C = ChannelGroupType::e50; channelGroupType == C)
    {
        return channelIsInGroup<C>(channelType);
    }
    if(constexpr auto C = ChannelGroupType::e51; channelGroupType == C)
    {
        return channelIsInGroup<C>(channelType);
    }
    if(constexpr auto C = ChannelGroupType::e61; channelGroupType == C)
    {
        return channelIsInGroup<C>(channelType);
    }
    if(constexpr auto C = ChannelGroupType::e71; channelGroupType == C)
    {
        return channelIsInGroup<C>(channelType);
    }
    return false;
}
}