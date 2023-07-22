#include "IAudioChannelGroup.hpp"

#include "util/Base.hpp"

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

template<ChannelGroupType Type>
ChannelType getChannelFromGroup(std::uint32_t index)
{
    return index < YADAW::Util::stackArraySize(speakers<Type>)?
        speakers<Type>[index]:
            ChannelType::Invalid;
}

namespace YADAW::Audio::Device
{
int IAudioChannelGroup::channelCount(ChannelGroupType channelGroupType)
{
    switch(channelGroupType)
    {
    case ChannelGroupType::eMono:
        return YADAW::Util::stackArraySize(speakers<ChannelGroupType::eMono>);
    case ChannelGroupType::eStereo:
        return YADAW::Util::stackArraySize(speakers<ChannelGroupType::eStereo>);
    case ChannelGroupType::eLRC:
        return YADAW::Util::stackArraySize(speakers<ChannelGroupType::eLRC>);
    case ChannelGroupType::eQuad:
        return YADAW::Util::stackArraySize(speakers<ChannelGroupType::eQuad>);
    case ChannelGroupType::e50:
        return YADAW::Util::stackArraySize(speakers<ChannelGroupType::e50>);
    case ChannelGroupType::e51:
        return YADAW::Util::stackArraySize(speakers<ChannelGroupType::e51>);
    case ChannelGroupType::e61:
        return YADAW::Util::stackArraySize(speakers<ChannelGroupType::e61>);
    case ChannelGroupType::e71:
        return YADAW::Util::stackArraySize(speakers<ChannelGroupType::e71>);
    default:
        return InvalidChannelCount;
    }
}

ChannelType IAudioChannelGroup::channelAt(
    ChannelGroupType channelGroupType, std::uint32_t index)
{
    switch(channelGroupType)
    {
    case ChannelGroupType::eMono:
        return getChannelFromGroup<ChannelGroupType::eMono>(index);
    case ChannelGroupType::eStereo:
        return getChannelFromGroup<ChannelGroupType::eStereo>(index);
    case ChannelGroupType::eLRC:
        return getChannelFromGroup<ChannelGroupType::eLRC>(index);
    case ChannelGroupType::eQuad:
        return getChannelFromGroup<ChannelGroupType::eQuad>(index);
    case ChannelGroupType::e50:
        return getChannelFromGroup<ChannelGroupType::e50>(index);
    case ChannelGroupType::e51:
        return getChannelFromGroup<ChannelGroupType::e51>(index);
    case ChannelGroupType::e61:
        return getChannelFromGroup<ChannelGroupType::e61>(index);
    case ChannelGroupType::e71:
        return getChannelFromGroup<ChannelGroupType::e71>(index);
    default:
        return ChannelType::Invalid;
    }
}

bool IAudioChannelGroup::isChannelInGroup(ChannelType channelType, ChannelGroupType channelGroupType)
{
    switch(channelGroupType)
    {
    case ChannelGroupType::eMono:
        return channelType == ChannelType::Center;
    case ChannelGroupType::eStereo:
        return channelType == ChannelType::Left
            || channelType == ChannelType::Right;
    case ChannelGroupType::eLRC:
        return channelType == ChannelType::Left
            || channelType == ChannelType::Right
            || channelType == ChannelType::FrontCenter;
    case ChannelGroupType::eQuad:
        return channelType == ChannelType::Left
            || channelType == ChannelType::Right
            || channelType == ChannelType::RearLeft
            || channelType == ChannelType::RearRight;
    case ChannelGroupType::e50:
        return channelType == ChannelType::Left
            || channelType == ChannelType::Center
            || channelType == ChannelType::Right
            || channelType == ChannelType::RearLeft
            || channelType == ChannelType::RearRight;
    case ChannelGroupType::e51:
        return channelType == ChannelType::Left
            || channelType == ChannelType::Center
            || channelType == ChannelType::Right
            || channelType == ChannelType::RearLeft
            || channelType == ChannelType::RearRight
            || channelType == ChannelType::LFE;
    case ChannelGroupType::e61:
        return channelType == ChannelType::Left
            || channelType == ChannelType::Center
            || channelType == ChannelType::Right
            || channelType == ChannelType::SideLeft
            || channelType == ChannelType::SideRight
            || channelType == ChannelType::RearCenter
            || channelType == ChannelType::LFE;
    case ChannelGroupType::e71:
        return channelType == ChannelType::Left
            || channelType == ChannelType::Center
            || channelType == ChannelType::Right
            || channelType == ChannelType::SideLeft
            || channelType == ChannelType::SideRight
            || channelType == ChannelType::RearLeft
            || channelType == ChannelType::RearRight
            || channelType == ChannelType::LFE;
    default:
        return false;
    }
}
}