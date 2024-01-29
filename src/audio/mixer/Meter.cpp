#include "Meter.hpp"

#include "audio/util/SquareSum.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Mixer
{
bool compareAbs(float lhs, float rhs)
{
    return std::abs(lhs) < std::abs(rhs);
}

Meter::Meter(
    std::uint32_t rmsWindowSize,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup):
    rmsWindowSize_(rmsWindowSize)
{
    input_.setChannelGroupType(channelGroupType, channelCountInGroup);
    auto channelCount = input_.channelCount();
    currentPeaks_.resize(channelCount, 0.0f);
    highestPeaks_.resize(channelCount, 0.0f);
    rms_.resize(channelCount, 0.0f);
    samples_.resize(channelCount,
        std::vector<float>(rmsWindowSize, 0.0f)
    );
}

std::uint32_t Meter::audioInputGroupCount() const
{
    return 1;
}

std::uint32_t Meter::audioOutputGroupCount() const
{
    return 0;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    Meter::audioInputGroupAt(std::uint32_t index) const
{
    return index == 0?
        OptionalAudioChannelGroup(std::ref(input_)):
        std::nullopt;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    Meter::audioOutputGroupAt(std::uint32_t index) const
{
    return std::nullopt;
}

std::uint32_t Meter::latencyInSamples() const
{
    return 0;
}

void Meter::process(const Device::AudioProcessData<float>& audioProcessData)
{
    const auto bufferSize = audioProcessData.singleBufferSize;
    if(auto firstSize = rmsWindowSize_ - index_;
        rmsWindowSize_ - index_ <= bufferSize)
    {
        FOR_RANGE0(i, input_.channelCount())
        {
            std::memcpy(samples_[i].data() + rmsWindowSize_ - index_,
                audioProcessData.inputs[0][i],
                bufferSize * sizeof(float)
            );
        }
        index_ += bufferSize;
        if(index_ == rmsWindowSize_)
        {
            index_ = 0;
        }
    }
    else
    {
        if(rmsWindowSize_ >= bufferSize)
        {
            auto secondSize = bufferSize - firstSize;
            FOR_RANGE0(i, input_.channelCount())
            {
                std::memcpy(samples_[i].data() + index_,
                    audioProcessData.inputs[0][i],
                    firstSize * sizeof(float)
                );
                std::memcpy(samples_[i].data(),
                    audioProcessData.inputs[0][i] + firstSize,
                    secondSize * sizeof(float)
                );
            }
            index_ = secondSize;
        }
        else // FIXME
        {
            auto secondSize = rmsWindowSize_ - firstSize;
            FOR_RANGE0(i, input_.channelCount())
            {
                std::memcpy(samples_[i].data() + index_,
                    audioProcessData.inputs[0][i] + bufferSize - rmsWindowSize_,
                    firstSize * sizeof(float)
                );
                std::memcpy(samples_[i].data(),
                    audioProcessData.inputs[0][i] + bufferSize - secondSize,
                    secondSize * sizeof(float)
                );
            }
        }
    }
    FOR_RANGE0(i, input_.channelCount())
    {
        currentPeaks_[i] = std::abs(
            *std::max_element(
                audioProcessData.inputs[0][i],
                audioProcessData.inputs[0][i] + bufferSize,
                &compareAbs
            )
        );
        rms_[i] = std::sqrt(
            YADAW::Audio::Util::squareSum(
                samples_[i].begin(), samples_[i].end()
            )
        );
    }
}

std::uint32_t Meter::audioChannelMapCount() const
{
    return 0;
}

YADAW::Audio::Device::IAudioDevice::OptionalChannelMap
    Meter::audioChannelMapAt(std::uint32_t index) const
{
    return std::nullopt;
}
}