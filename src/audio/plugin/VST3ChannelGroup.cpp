#include "VST3ChannelGroup.hpp"

namespace YADAW::Audio::Plugin
{
using namespace YADAW::Audio::Device;

VST3ChannelGroup::VST3ChannelGroup():
    speakerArrangement_(SpeakerArr::kEmpty),
    busInfo_()
{
}

VST3ChannelGroup::VST3ChannelGroup(SpeakerArrangement speakerArrangement, BusInfo busInfo):
    speakerArrangement_(speakerArrangement),
    busInfo_(busInfo)
{
}

VST3ChannelGroup& VST3ChannelGroup::operator=(VST3ChannelGroup rhs)
{
    std::swap(*this, rhs);
    return *this;
}

QString VST3ChannelGroup::name() const
{
    return QString::fromUtf16(busInfo_.name);
}

std::uint8_t VST3ChannelGroup::channelCount() const
{
    // Alternative solution
    // return SpeakerArr::getChannelCount(speakerArrangement_);
    return busInfo_.channelCount;
}

YADAW::Audio::Base::ChannelGroupType VST3ChannelGroup::type() const
{
    return speakerArrangement_ == SpeakerArr::kStereo? YADAW::Audio::Base::ChannelGroupType::Stereo:
        speakerArrangement_ == SpeakerArr::kMono? YADAW::Audio::Base::ChannelGroupType::Mono:
        YADAW::Audio::Base::ChannelGroupType::Custom;
}

YADAW::Audio::Base::ChannelType VST3ChannelGroup::speakerAt(std::uint8_t index) const
{
    auto speaker = SpeakerArr::getSpeaker(speakerArrangement_, index);
    if(speaker == Steinberg::Vst::kSpeakerL)
    {
        return YADAW::Audio::Base::ChannelType::Left;
    }
    else if(speaker == Steinberg::Vst::kSpeakerR)
    {
        return YADAW::Audio::Base::ChannelType::Right;
    }
    else if(speaker == 0)
    {
        return YADAW::Audio::Base::ChannelType::Invalid;
    }
    return index < channelCount()? YADAW::Audio::Base::ChannelType::Custom: YADAW::Audio::Base::ChannelType::Invalid;
}

QString VST3ChannelGroup::speakerNameAt(std::uint8_t index) const
{
    return SpeakerArr::getSpeakerShortName(speakerArrangement_, index);
}

bool VST3ChannelGroup::isMain() const
{
    return busInfo_.busType == BusTypes::kMain;
}
}