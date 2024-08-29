#ifndef YADAW_SRC_AUDIO_MIXER_POLARITYINVERTER
#define YADAW_SRC_AUDIO_MIXER_POLARITYINVERTER

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

namespace YADAW::Audio::Mixer
{
class PolarityInverter: public YADAW::Audio::Device::IAudioDevice
{
public:
    PolarityInverter(
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0);
    ~PolarityInverter();
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const Device::AudioProcessData<float>& audioProcessData) override;
public:
    std::uint64_t inverted() const;
    void setInverted(std::uint64_t inverted);
private:
    YADAW::Audio::Util::AudioChannelGroup input_;
    YADAW::Audio::Util::AudioChannelGroup output_;
    std::atomic_uint64_t invert_ = 0;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_POLARITYINVERTER