#ifndef YADAW_SRC_AUDIO_MIXER_BLANKGENERATOR
#define YADAW_SRC_AUDIO_MIXER_BLANKGENERATOR

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

namespace YADAW::Audio::Mixer
{
class BlankGenerator: public YADAW::Audio::Device::IAudioDevice
{
public:
    BlankGenerator(YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0);
    BlankGenerator(const BlankGenerator&) = default;
    BlankGenerator& operator=(const BlankGenerator&) = default;
    ~BlankGenerator() noexcept = default;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;

private:
    YADAW::Audio::Util::AudioChannelGroup output_;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_BLANKGENERATOR
