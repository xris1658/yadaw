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
    output_.setChannelGroupType(channelGroupType, channelCountInGroup);
    auto channelCount = input_.channelCount();
    currentPeaks_.resize(channelCount, 0.0f);
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
    return 1;
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
    return index == 0?
        OptionalAudioChannelGroup(std::ref(output_)):
        std::nullopt;
}

std::uint32_t Meter::latencyInSamples() const
{
    return 0;
}

void Meter::process(const Device::AudioProcessData<float>& audioProcessData)
{
    const auto bufferSize = audioProcessData.singleBufferSize;
    FOR_RANGE0(i, input_.channelCount())
    {
        std::memmove(
            audioProcessData.outputs[0][i],
            audioProcessData.inputs[0][i],
            sizeof(float) * bufferSize
        );
    }
    if(accessingPeak_.try_lock())
    {
        FOR_RANGE0(i, input_.channelCount())
        {
            currentPeaks_[i] = std::abs(
                *std::max_element(
                    audioProcessData.inputs[0][i],
                    audioProcessData.inputs[0][i] + bufferSize,
                    &compareAbs
                )
            );
        }
        accessingPeak_.unlock();
    }
    if(accessingRMS_.try_lock())
    {
        if(bufferSize >= rmsWindowSize_)
        {
            FOR_RANGE0(i, input_.channelCount())
            {
                std::memcpy(samples_[i].data(),
                    audioProcessData.inputs[0][i] + (bufferSize - rmsWindowSize_),
                    sizeof(bufferSize) * sizeof(float)
                );
            }
            index_ = 0;
        }
        else
        {
            auto oldIndex = index_;
            index_ += bufferSize;
            if(index_ >= rmsWindowSize_)
            {
                FOR_RANGE0(i, input_.channelCount())
                {
                    std::memcpy(samples_[i].data() + oldIndex,
                        audioProcessData.inputs[0][i],
                        (rmsWindowSize_ - oldIndex) * sizeof(float)
                    );
                    std::memcpy(samples_[i].data(),
                        audioProcessData.inputs[0][i] + (rmsWindowSize_ - oldIndex),
                        (index_ - rmsWindowSize_) * sizeof(float)
                    );
                }
                index_ -= rmsWindowSize_;
            }
            else
            {
                FOR_RANGE0(i, input_.channelCount())
                {
                    std::memcpy(samples_[i].data() + oldIndex,
                        audioProcessData.inputs[0][i],
                        bufferSize * sizeof(float)
                    );
                }

            }
        }
        FOR_RANGE0(i, input_.channelCount())
        {
            rms_[i] = std::sqrt(
                YADAW::Audio::Util::squareSum(
                    samples_[i].begin(), samples_[i].end()
                )
            );
        }
        accessingRMS_.unlock();
    }
}

std::pair<const float*, std::unique_lock<YADAW::Util::AtomicMutex>> Meter::currentPeaks() const
{
    return {
        currentPeaks_.data(),
        std::unique_lock<YADAW::Util::AtomicMutex>(accessingPeak_)
    };
}

void Meter::resetPeak()
{
    std::unique_lock<YADAW::Util::AtomicMutex> lock(accessingPeak_);
    std::fill(currentPeaks_.begin(), currentPeaks_.end(), 0.0f);
}

std::uint32_t Meter::rmsWindowSize() const
{
    return rmsWindowSize_;
}

std::pair<const float*, std::unique_lock<YADAW::Util::AtomicMutex>> Meter::currentRMS() const
{
    return {
        rms_.data(),
        std::unique_lock<YADAW::Util::AtomicMutex>(accessingRMS_)
    };
}

void Meter::setRMSWindowSize(std::uint32_t rmsWindowSize)
{
    std::unique_lock<YADAW::Util::AtomicMutex> lock(accessingRMS_);
    FOR_RANGE0(i, samples_.size())
    {
        samples_[i].clear();
        samples_[i].shrink_to_fit();
        samples_[i].resize(rmsWindowSize, 0.0f);
    }
    for(auto& rms: rms_)
    {
        rms = 0.0f;
    }
    index_ = 0;
}

void Meter::resetRMS()
{
    std::unique_lock<YADAW::Util::AtomicMutex> lock(accessingRMS_);
    for(auto& rms: rms_)
    {
        rms = 0.0f;
    }
}
}