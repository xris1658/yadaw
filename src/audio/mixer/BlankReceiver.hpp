#ifndef YADAW_SRC_AUDIO_MIXER_BLANKRECEIVER
#define YADAW_SRC_AUDIO_MIXER_BLANKRECEIVER

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

namespace YADAW::Audio::Mixer
{
class BlankReceiver: public YADAW::Audio::Device::IAudioDevice
{
public:
    BlankReceiver(YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup = 0);
    BlankReceiver(const BlankReceiver&) = default;
    BlankReceiver& operator=(const BlankReceiver&) = default;
    ~BlankReceiver() noexcept = default;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;

private:
    YADAW::Audio::Util::AudioChannelGroup input_;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_BLANKRECEIVER
