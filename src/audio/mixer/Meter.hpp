#ifndef YADAW_SRC_AUDIO_MIXER_METER
#define YADAW_SRC_AUDIO_MIXER_METER

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"
#include "util/AtomicMutex.hpp"

#include <mutex>
#include <utility>

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
public:
    std::pair<
        const float*,
        std::unique_lock<YADAW::Util::AtomicMutex>
    > currentPeaks() const;
    void resetPeak();
    std::uint32_t rmsWindowSize() const;
    std::pair<
        const float*,
        std::unique_lock<YADAW::Util::AtomicMutex>
    > currentRMS() const;
    void setRMSWindowSize(std::uint32_t rmsWindowSize);
    void resetRMS();
private:
    YADAW::Audio::Util::AudioChannelGroup input_;
    YADAW::Audio::Util::AudioChannelGroup output_;
    std::vector<float> currentPeaks_;
    std::vector<float> rms_;
    std::uint32_t rmsWindowSize_;
    std::uint32_t index_ = 0;
    mutable YADAW::Util::AtomicMutex accessingPeak_;
    mutable YADAW::Util::AtomicMutex accessingRMS_;
    std::vector<std::vector<float>> samples_;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_METER
