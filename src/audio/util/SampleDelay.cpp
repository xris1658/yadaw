#include "SampleDelay.hpp"

namespace YADAW::Audio::Util
{
SampleDelay::AudioChannelGroup::AudioChannelGroup(const Device::IAudioChannelGroup& group):
    name_(group.name()),
    type_(group.type()),
    speakers_(group.channelCount()),
    speakerNames_(group.channelCount())
{
    for(std::size_t i = 0; i < speakers_.size(); ++i)
    {
        speakers_[i] = group.speakerAt(i);
        speakerNames_[i] = group.speakerNameAt(i);
    }
}

QString SampleDelay::AudioChannelGroup::name() const
{
    return name_;
}

std::uint32_t SampleDelay::AudioChannelGroup::channelCount() const
{
    return speakers_.size();
}

YADAW::Audio::Base::ChannelGroupType SampleDelay::AudioChannelGroup::type() const
{
    return type_;
}

YADAW::Audio::Base::ChannelType SampleDelay::AudioChannelGroup::speakerAt(std::uint32_t index) const
{
    return index < channelCount()? speakers_[index]: YADAW::Audio::Base::ChannelType::Invalid;
}

QString SampleDelay::AudioChannelGroup::speakerNameAt(std::uint32_t index) const
{
    return index < channelCount()? speakerNames_[index]: QString();
}

bool SampleDelay::AudioChannelGroup::isMain() const
{
    return true;
}

SampleDelay::SampleDelay(std::uint32_t delay,
    const YADAW::Audio::Device::IAudioChannelGroup& channelGroup):
    delay_(delay),
    processing_(false),
    offset_(0),
    buffers_(channelGroup.channelCount(), std::vector<float>(delay, 0.0f)),
    channelGroup_(channelGroup)
{}

SampleDelay::~SampleDelay() noexcept
{
    stopProcessing();
}

bool SampleDelay::setDelay(std::uint32_t delay)
{
    if(delay_ == delay)
    {
        return true;
    }
    if(!processing_)
    {
        std::vector<std::vector<float>> buffers(buffers_.size(),
            std::vector<float>(delay, 0.0f));
        // 5 -> 3:
        // old:           v
        //      | 1 | 2 | 3 | 4 | 5 |
        //
        // new:   v
        //      | 5 | 1 | 2 |
        if(delay_ > delay)
        {
            const auto delta = delay_ - delay;
            for(std::size_t i = 0; i < buffers_.size(); ++i)
            {
                for(std::size_t j = 0; j < delay; ++j)
                {
                    buffers[i][j] = buffers_[i][(offset_ + j + delta) % delay_];
                }
            }
            offset_ = 0;
        }
        // 3 -> 5:
        // old:       v
        //      | 1 | 2 | 3 |
        //
        // new:   v
        //      | 0 | 0 | 2 | 3 | 1 |
        else
        {
            const auto delta = delay - delay_;
            for(std::size_t i = 0; i < buffers_.size(); ++i)
            {
                for(std::size_t j = 0; j < delay_; ++j)
                {
                    buffers[i][j + delta] = buffers_[i][(offset_ + j) % delay_];
                }
            }
            offset_ = 0;
        }
        buffers_ = std::move(buffers);
        delay_ = delay;
        return true;
    }
    return false;
}

bool SampleDelay::startProcessing()
{
    if(delay_ != 0)
    {
        processing_ = true;
        return true;
    }
    return false;
}

bool SampleDelay::stopProcessing()
{
    processing_ = false;
    return true;
}

uint32_t SampleDelay::audioInputGroupCount() const
{
    return 1;
}

uint32_t SampleDelay::audioOutputGroupCount() const
{
    return 1;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    SampleDelay::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(
            static_cast<const YADAW::Audio::Device::IAudioChannelGroup&>(channelGroup_)
        ):
        std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    SampleDelay::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(
            static_cast<const YADAW::Audio::Device::IAudioChannelGroup&>(channelGroup_)
        ):
        std::nullopt;
}

uint32_t SampleDelay::latencyInSamples() const
{
    return delay_;
}

void SampleDelay::process(const Device::AudioProcessData<float>& audioProcessData)
{
    if(processing_)
    {
        for(std::uint32_t i = 0; i < audioProcessData.outputCounts[0]; ++i)
        {
            for(std::uint32_t j = 0; j < audioProcessData.singleBufferSize; ++j)
            {
                auto input = audioProcessData.inputs[0][i][j];
                audioProcessData.outputs[0][i][j] = buffers_[i][(j + offset_) % delay_];
                buffers_[i][(j + offset_) % delay_] = input;
            }
        }
        offset_ += audioProcessData.singleBufferSize;
        if(offset_ > delay_)
        {
            offset_ %= delay_;
        }
    }
}
}
