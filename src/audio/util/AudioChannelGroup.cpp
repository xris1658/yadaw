#include "AudioChannelGroup.hpp"

#include "util/IntegerRange.hpp"

#include <stdexcept>

namespace YADAW::Audio::Util
{
AudioChannelGroup::AudioChannelGroup()
{}

AudioChannelGroup AudioChannelGroup::from(const Device::IAudioChannelGroup& rhs)
{
    AudioChannelGroup ret;
    ret.name_ = rhs.name();
    ret.type_ = rhs.type();
    ret.isMain_ = rhs.isMain();
    ret.speakers_.resize(rhs.channelCount());
    FOR_RANGE0(i, ret.speakers_.size())
    {
        ret.speakers_[i].first = rhs.speakerAt(i);
        ret.speakers_[i].second = rhs.speakerNameAt(i);
    }
    return ret;
}

QString AudioChannelGroup::name() const
{
    return name_;
}

std::uint32_t AudioChannelGroup::channelCount() const
{
    return speakers_.size();
}

YADAW::Audio::Base::ChannelGroupType AudioChannelGroup::type() const
{
    return type_;
}

YADAW::Audio::Base::ChannelType AudioChannelGroup::speakerAt(std::uint32_t index) const
{
    return index < speakers_.size()?
        speakers_[index].first:
        YADAW::Audio::Base::ChannelType::Invalid;
}

QString AudioChannelGroup::speakerNameAt(std::uint32_t index) const
{
    return index < speakers_.size()?
        speakers_[index].second:
        QString();
}

bool AudioChannelGroup::isMain() const
{
    return isMain_;
}

void AudioChannelGroup::setName(const QString& name)
{
    name_ = name;
}

void AudioChannelGroup::setName(QString&& name)
{
    name_ = std::move(name);
}

void AudioChannelGroup::setMain(bool isMain)
{
    isMain_ = isMain;
}

void AudioChannelGroup::setChannelGroupType(YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCount)
{
    speakers_.clear();
    if(channelCount == 0)
    {
        auto actualChannelCount = YADAW::Audio::Device::IAudioChannelGroup::channelCount(channelGroupType);
        if(actualChannelCount == YADAW::Audio::Base::InvalidChannelCount)
        {
            throw std::invalid_argument("The channel count of the specified group type is unknown");
        }
        channelCount = actualChannelCount;
    }
    speakers_.reserve(channelCount);
    FOR_RANGE0(i, channelCount)
    {
        speakers_.emplace_back(
            std::make_pair(
                IAudioChannelGroup::channelAt(channelGroupType, i),
                QString()
            )
        );
    }
    type_ = channelGroupType;
}

bool AudioChannelGroup::setSpeakerType(std::uint32_t index, YADAW::Audio::Base::ChannelType type)
{
    if(index < speakers_.size())
    {
        speakers_[index].first = type;
        return true;
    }
    return false;
}

bool AudioChannelGroup::setSpeakerName(std::uint32_t index, const QString& name)
{
    if(index < speakers_.size())
    {
        speakers_[index].second = name;
        return true;
    }
    return false;
}

bool AudioChannelGroup::setSpeakerName(std::uint32_t index, QString&& name)
{
    if(index < speakers_.size())
    {
        speakers_[index].second = std::move(name);
        return true;
    }
    return false;
}
}