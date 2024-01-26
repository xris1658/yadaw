#ifndef YADAW_SRC_AUDIO_MIXER_METER
#define YADAW_SRC_AUDIO_MIXER_METER

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

namespace YADAW::Audio::Mixer
{
class Meter: public YADAW::Audio::Device::IAudioDevice
{
public:
    Meter(std::uint32_t rmsWindowSize,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0);
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
    std::uint32_t audioChannelMapCount() const override;
    OptionalChannelMap audioChannelMapAt(std::uint32_t index) const override;
public:
    float currentPeak(std::uint32_t index) const;
    float currentPeakAt(std::uint32_t index) const;
    std::uint32_t rmsWindowSize() const;
    bool setRMSWindowSize(std::uint32_t rmsWindowSize);
    float rms(std::uint32_t index) const;
    float rmsAt(std::uint32_t index) const;
    void resetRMS();
private:
    YADAW::Audio::Util::AudioChannelGroup input_;
    std::vector<float> currentPeaks_;
    std::vector<float> highestPeaks_;
    std::vector<float> rms_;
    std::uint32_t rmsWindowSize_;
    std::uint32_t index_ = 0;
    std::vector<std::vector<float>> samples_;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_METER
