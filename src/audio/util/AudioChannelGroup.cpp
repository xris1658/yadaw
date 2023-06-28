#include "AudioChannelGroup.hpp"

namespace YADAW::Audio::Util
{
AudioChannelGroup::AudioChannelGroup()
{

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
        speakers_[index]:
        YADAW::Audio::Base::ChannelType::Invalid;
}

QString AudioChannelGroup::speakerNameAt(std::uint32_t index) const
{
    return index < speakers_.size()?
        speakerNames_[index]:
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
    auto channelCountOfType = YADAW::Audio::Device::IAudioChannelGroup::channelCount(channelGroupType);
    if(channelCountOfType == -1)
    {
        speakers_.clear();
        speakerNames_.clear();
        speakers_.resize(channelCount, YADAW::Audio::Base::ChannelType::Custom);
        speakerNames_.resize(channelCount, QString());
    }
    else if(channelCount == 0)
    {
        channelCount = channelCountOfType;
        speakers_.clear();
        speakerNames_.clear();
        for(decltype(channelCount) i = 0; i < channelCount; ++i)
        {
            speakers_.emplace_back(IAudioChannelGroup::channelAt(channelGroupType, i));
        }
        speakers_.resize(channelCount, YADAW::Audio::Base::ChannelType::Custom);
        speakerNames_.resize(channelCount);
    }
}

bool AudioChannelGroup::setSpeakerType(std::uint32_t index, YADAW::Audio::Base::ChannelType type)
{
    if(index < speakers_.size())
    {
        speakers_[index] = type;
        return true;
    }
    return false;
}

bool AudioChannelGroup::setSpeakerName(std::uint32_t index, const QString& name)
{
    if(index < speakerNames_.size())
    {
        speakerNames_[index] = name;
        return true;
    }
    return false;
}

bool AudioChannelGroup::setSpeakerName(std::uint32_t index, QString&& name)
{
    if(index < speakerNames_.size())
    {
        speakerNames_[index] = std::move(name);
        return true;
    }
    return false;
}
}