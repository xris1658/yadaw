#include "VST3AudioChannelGroup.hpp"

namespace YADAW::Audio::Plugin
{
using namespace YADAW::Audio::Base;
using namespace YADAW::Audio::Device;

using namespace Steinberg::Vst;

template<Steinberg::Vst::SpeakerArrangement Arr>
ChannelType vst3Speakers[] = {};

template<> ChannelType vst3Speakers<SpeakerArr::kMono>[] = {
    ChannelType::Center
};

template<> ChannelType vst3Speakers<SpeakerArr::kStereo>[] = {
    ChannelType::Left, ChannelType::Right
};

// pluginterfaces/vst/vstspeaker.h

template<> ChannelType vst3Speakers<SpeakerArr::k30Cine>[] = {
    ChannelType::Left, ChannelType::Right, ChannelType::Center
};

template<> ChannelType vst3Speakers<SpeakerArr::k40Music>[] = {
    ChannelType::Left, ChannelType::Right,
    ChannelType::RearLeft, ChannelType::RearRight
};

template<> ChannelType vst3Speakers<SpeakerArr::k50>[] = {
    ChannelType::Left, ChannelType::Right,
    ChannelType::Center,
    ChannelType::RearLeft, ChannelType::RearRight
};

template<> ChannelType vst3Speakers<SpeakerArr::k51>[] = {
    ChannelType::Left, ChannelType::Right,
    ChannelType::Center,
    ChannelType::LFE,
    ChannelType::RearLeft, ChannelType::RearRight
};

template<> ChannelType vst3Speakers<SpeakerArr::k61Cine>[] = {
    ChannelType::Left, ChannelType::Right,
    ChannelType::Center,
    ChannelType::LFE,
    ChannelType::RearLeft, ChannelType::RearRight, ChannelType::RearCenter
};

template<> ChannelType vst3Speakers<SpeakerArr::k71Cine>[] = {
    ChannelType::Left, ChannelType::Right,
    ChannelType::Center,
    ChannelType::LFE,
    ChannelType::RearLeft, ChannelType::RearRight,
    ChannelType::SideLeft, ChannelType::SideRight
};

VST3AudioChannelGroup::VST3AudioChannelGroup():
    speakerArrangement_(SpeakerArr::kEmpty),
    busInfo_()
{
}

VST3AudioChannelGroup::VST3AudioChannelGroup(SpeakerArrangement speakerArrangement, BusInfo busInfo):
    speakerArrangement_(speakerArrangement),
    busInfo_(busInfo)
{
}

VST3AudioChannelGroup& VST3AudioChannelGroup::operator=(VST3AudioChannelGroup rhs)
{
    std::swap(*this, rhs);
    return *this;
}

QString VST3AudioChannelGroup::name() const
{
    return QString::fromUtf16(busInfo_.name);
}

std::uint32_t VST3AudioChannelGroup::channelCount() const
{
    return busInfo_.channelCount;
}

YADAW::Audio::Base::ChannelGroupType VST3AudioChannelGroup::type() const
{
    switch(speakerArrangement_)
    {
    case SpeakerArr::kEmpty:
        return YADAW::Audio::Base::ChannelGroupType::eEmpty;
    case SpeakerArr::kMono:
        return YADAW::Audio::Base::ChannelGroupType::eMono;
    case SpeakerArr::kStereo:
        return YADAW::Audio::Base::ChannelGroupType::eStereo;
    case SpeakerArr::k30Cine:
        return YADAW::Audio::Base::ChannelGroupType::eLRC;
    case SpeakerArr::k40Music:
        return YADAW::Audio::Base::ChannelGroupType::eQuad;
    case SpeakerArr::k50:
        return YADAW::Audio::Base::ChannelGroupType::e50;
    case SpeakerArr::k51:
        return YADAW::Audio::Base::ChannelGroupType::e51;
    case SpeakerArr::k61Cine:
        return YADAW::Audio::Base::ChannelGroupType::e61;
    case SpeakerArr::k71Cine:
        return YADAW::Audio::Base::ChannelGroupType::e71;
    default:
        return YADAW::Audio::Base::ChannelGroupType::eCustomGroup;
    }
}

YADAW::Audio::Base::ChannelType VST3AudioChannelGroup::speakerAt(std::uint32_t index) const
{
    using namespace YADAW::Audio::Base;
    using namespace Steinberg::Vst;
    if(index < channelCount())
    {
        auto speaker = SpeakerArr::getSpeaker(speakerArrangement_, index);
        switch(speaker)
        {
        case kSpeakerM:
        case kSpeakerC:
            return ChannelType::Center;
        case kSpeakerL:
            return ChannelType::Left;
        case kSpeakerR:
            return ChannelType::Right;
        case kSpeakerLs:
            return ChannelType::RearLeft;
        case kSpeakerRs:
            return ChannelType::RearRight;
        case kSpeakerCs:
            return ChannelType::RearCenter;
        case kSpeakerLfe:
            return ChannelType::LFE;
        case kSpeakerSl:
            return ChannelType::SideLeft;
        case kSpeakerSr:
            return ChannelType::SideRight;
        case 0:
            return ChannelType::Invalid;
        default:
            return ChannelType::Custom;
        }
    }
    return YADAW::Audio::Base::ChannelType::Invalid;
}

QString VST3AudioChannelGroup::speakerNameAt(std::uint32_t index) const
{
    return SpeakerArr::getSpeakerShortName(speakerArrangement_, index);
}

bool VST3AudioChannelGroup::isMain() const
{
    return busInfo_.busType == BusTypes::kMain;
}
}