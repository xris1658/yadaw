#include "SampleDelay.hpp"

#include <array>
#include <algorithm>
#include <ranges>

template<typename T>
auto rotateAndTakeLast(
    const std::vector<T>& from,
    std::size_t offset, std::size_t itemCount,
    std::vector<T>& to
)
{
    auto sizes = itemCount > offset?
        std::array{itemCount - offset, offset}:
        std::array{static_cast<std::size_t>(0), itemCount};
    auto copyFrom = {
        std::ranges::views::counted(from.end() - sizes[0], sizes[0]),
        std::ranges::views::counted(from.begin() + offset - sizes[1], sizes[1])
    };
    return std::ranges::copy(
        std::ranges::views::join(copyFrom), to.begin()
    ).out;
}

namespace YADAW::Audio::Util
{
SampleDelay::SampleDelay(std::uint32_t delay,
    const YADAW::Audio::Device::IAudioChannelGroup& channelGroup):
    delay_(delay),
    processing_(false),
    processFunc_(delay_ == 0?
        &SampleDelay::doProcessIfDelayIsZero:
        &SampleDelay::doProcessIfDelayIsNotZero
    ),
    offset_(0),
    buffers_(channelGroup.channelCount(), std::vector<float>(delay, 0.0f)),
    channelGroup_(AudioChannelGroup::from(channelGroup))
{}

SampleDelay::SampleDelay(SampleDelay&& rhs) noexcept:
    delay_(rhs.delay_),
    processing_(rhs.processing_),
    processFunc_(rhs.processFunc_),
    offset_(rhs.offset_),
    buffers_(std::move(rhs.buffers_)),
    channelGroup_(std::move(rhs.channelGroup_))
{
    rhs.delay_ = 0;
    rhs.offset_ = 0;
    rhs.processing_ = false;
    rhs.processFunc_ = &SampleDelay::doProcessIfDelayIsZero;
}

SampleDelay& SampleDelay::operator=(SampleDelay&& rhs) noexcept
{
    if(this != &rhs)
    {
        delay_ = rhs.delay_;
        processing_ = rhs.processing_;
        processFunc_ = rhs.processFunc_;
        offset_ = rhs.offset_;
        buffers_ = std::move(rhs.buffers_);
        channelGroup_ = std::move(rhs.channelGroup_);
        rhs.delay_ = 0;
        rhs.offset_ = 0;
        rhs.processing_ = false;
        rhs.processFunc_ = &SampleDelay::doProcessIfDelayIsZero;
    }
    return *this;
}

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
            for(std::size_t i = 0; i < buffers_.size(); ++i)
            {
                rotateAndTakeLast(buffers_[i], offset_, delay, buffers[i]);
            }
        }
        // 3 -> 5:
        // old:       v
        //      | 1 | 2 | 3 |
        //
        // new:   v
        //      | 0 | 0 | 2 | 3 | 1 |
        else
        {
            for(std::size_t i = 0; i < buffers_.size(); ++i)
            {
                const auto delta = delay - delay_;
                std::ranges::rotate_copy(
                    buffers_[i],
                    buffers_[i].begin() + offset_,
                    buffers[i].begin() + delta
                );
            }
        }
        offset_ = 0;
        buffers_ = std::move(buffers);
        delay_ = delay;
        processFunc_ = delay_ == 0? &SampleDelay::doProcessIfDelayIsZero:
            &SampleDelay::doProcessIfDelayIsNotZero;
        return true;
    }
    return false;
}

bool SampleDelay::isProcessing() const
{
    return processing_;
}

bool SampleDelay::startProcessing()
{
    processing_ = true;
    return true;
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
        (this->*processFunc_)(audioProcessData);
    }
}

void SampleDelay::doProcessIfDelayIsZero(const Device::AudioProcessData<float>& audioProcessData)
{
    for(std::uint32_t i = 0; i < audioProcessData.outputCounts[0]; ++i)
    {
        if(audioProcessData.outputs[0][i] != audioProcessData.inputs[0][i])
        {
            for(std::uint32_t j = 0; j < audioProcessData.singleBufferSize; ++j)
            {
                audioProcessData.outputs[0][i][j] = audioProcessData.inputs[0][i][j];
            }
        }
    }
}

void SampleDelay::doProcessIfDelayIsNotZero(const Device::AudioProcessData<float>& audioProcessData)
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
