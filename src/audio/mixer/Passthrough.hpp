#ifndef YADAW_SRC_AUDIO_MIXER_PASSTHROUGH
#define YADAW_SRC_AUDIO_MIXER_PASSTHROUGH

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

namespace YADAW::Audio::Mixer
{
class Passthrough: public YADAW::Audio::Device::IAudioDevice
{
public:
    Passthrough(YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup = 0);
    Passthrough(const Passthrough&) = default;
    Passthrough& operator=(const Passthrough&) = default;
    ~Passthrough() noexcept = default;
public:
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
private:
    YADAW::Audio::Util::AudioChannelGroup input_;
    YADAW::Audio::Util::AudioChannelGroup output_;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_PASSTHROUGH
