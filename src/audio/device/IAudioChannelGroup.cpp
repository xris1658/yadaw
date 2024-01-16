#include "IAudioChannelGroup.hpp"

#include "util/Base.hpp"

#include <iterator> // std::size

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
    int channelCountArray[] = {
        std::size(speakers<ChannelGroupType::eMono>),
        std::size(speakers<ChannelGroupType::eStereo>),
        std::size(speakers<ChannelGroupType::eLRC>),
        std::size(speakers<ChannelGroupType::eQuad>),
        std::size(speakers<ChannelGroupType::e50>),
        std::size(speakers<ChannelGroupType::e51>),
        std::size(speakers<ChannelGroupType::e61>),
        std::size(speakers<ChannelGroupType::e71>)
    };
    return channelGroupType > ChannelGroupType::eEmpty
        && channelGroupType < ChannelGroupType::eEnd?
        channelCountArray[YADAW::Util::underlyingValue(channelGroupType) - 1]:
        InvalidChannelCount;
}

ChannelType IAudioChannelGroup::channelAt(
    ChannelGroupType channelGroupType, std::uint32_t index)
{
    using GetChannelAtFunc = ChannelType(std::uint32_t);
    GetChannelAtFunc* func[] = {
        &getChannelFromGroup<ChannelGroupType::eMono>,
        &getChannelFromGroup<ChannelGroupType::eStereo>,
        &getChannelFromGroup<ChannelGroupType::eLRC>,
        &getChannelFromGroup<ChannelGroupType::eQuad>,
        &getChannelFromGroup<ChannelGroupType::e50>,
        &getChannelFromGroup<ChannelGroupType::e51>,
        &getChannelFromGroup<ChannelGroupType::e61>,
        &getChannelFromGroup<ChannelGroupType::e71>
    };
    return channelGroupType > ChannelGroupType::eEmpty
        && channelGroupType < ChannelGroupType::eEnd?
        func[YADAW::Util::underlyingValue(channelGroupType) - 1](index):
        ChannelType::Invalid;
}

bool IAudioChannelGroupchannelIsInGroup(
    ChannelType channelType, ChannelGroupType channelGroupType)
{
    using ChannelIsInGroupFunc = bool(ChannelType);
    ChannelIsInGroupFunc* func[] = {
        &channelIsInGroup<ChannelGroupType::eMono>,
        &channelIsInGroup<ChannelGroupType::eStereo>,
        &channelIsInGroup<ChannelGroupType::eLRC>,
        &channelIsInGroup<ChannelGroupType::eQuad>,
        &channelIsInGroup<ChannelGroupType::e50>,
        &channelIsInGroup<ChannelGroupType::e51>,
        &channelIsInGroup<ChannelGroupType::e61>,
        &channelIsInGroup<ChannelGroupType::e71>
    };
    return channelGroupType > ChannelGroupType::eEmpty
        && channelGroupType < ChannelGroupType::eEnd?
        func[YADAW::Util::underlyingValue(channelGroupType) - 1](channelType):
        false;
}
}