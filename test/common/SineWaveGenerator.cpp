#include "SineWaveGenerator.hpp"


SineWaveGenerator::SineWaveGenerator(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup):
channelGroup_(),
frequency_(0)
{
    channelGroup_.setChannelGroupType(channelGroupType, channelCountInGroup);
}

double SineWaveGenerator::frequency() const
{
    return frequency_;
}
double SineWaveGenerator::sampleRate() const
{
    return sampleRate_;
}
bool SineWaveGenerator::setFrequency(double frequency)
{
    if(!isProcessing())
    {
        frequency_ = frequency;
        return true;
    }
    return false;
}
bool SineWaveGenerator::setSampleRate(double sampleRate)
{
    if(!isProcessing())
    {
        sampleRate_ = sampleRate;
        return true;
    }
    return false;
}
bool SineWaveGenerator::isProcessing() const
{
    return processing_;
}
bool SineWaveGenerator::startProcessing()
{
    processing_ = true;
    return true;
}
bool SineWaveGenerator::stopProcessing()
{
    processing_ = false;
    return true;
}

std::uint32_t SineWaveGenerator::audioInputGroupCount() const
{
    return 0;
}
std::uint32_t SineWaveGenerator::audioOutputGroupCount() const
{
    return 1;
}
YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup SineWaveGenerator::audioInputGroupAt(std::uint32_t index) const
{
    return std::nullopt;
}
YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup SineWaveGenerator::audioOutputGroupAt(std::uint32_t index) const
{
    return index == 0? OptionalAudioChannelGroup(channelGroup_): std::nullopt;
}
std::uint32_t SineWaveGenerator::latencyInSamples() const
{
    return 0U;
}
void SineWaveGenerator::process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData)
{
    if(processing_)
    {
        auto cycle = sampleRate_ / frequency_;
        const auto _2pi = 2.0f * std::acos(-1.0f);
        FOR_RANGE0(j, audioProcessData.singleBufferSize)
        {
            auto value = std::sin(_2pi * (frameCount_ + j) / cycle);
            FOR_RANGE0(i, channelGroup_.channelCount())
            {
                audioProcessData.outputs[0][i][j] = value;
            }
        }
        frameCount_ += audioProcessData.singleBufferSize;
    }
}

std::uint32_t SineWaveGenerator::audioChannelMapCount() const
{
    return 0;
}

YADAW::Audio::Device::IAudioDevice::OptionalChannelMap
    SineWaveGenerator::audioChannelMapAt(std::uint32_t index) const
{
    return std::nullopt;
}
