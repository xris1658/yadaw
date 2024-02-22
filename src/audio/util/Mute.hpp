#ifndef YADAW_SRC_AUDIO_UTIL_MUTE
#define YADAW_SRC_AUDIO_UTIL_MUTE

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"
#include "util/AtomicMutex.hpp"

namespace YADAW::Audio::Util
{
class Mute: public YADAW::Audio::Device::IAudioDevice
{
public:
    Mute(YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0);
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
public:
    void setMute(bool mute);
private:
    YADAW::Audio::Util::AudioChannelGroup input_;
    YADAW::Audio::Util::AudioChannelGroup output_;
    bool mute_ = false;
    YADAW::Util::AtomicMutex usingMute_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_MUTE
