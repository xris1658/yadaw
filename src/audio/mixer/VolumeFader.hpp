#ifndef YADAW_SRC_AUDIO_MIXER_VOLUMEFADER
#define YADAW_SRC_AUDIO_MIXER_VOLUMEFADER

#include "audio/base/Automation.hpp"
#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

namespace YADAW::Audio::Mixer
{
class VolumeFader: public YADAW::Audio::Device::IAudioDevice
{
private:
    using ProcessFunc = void(VolumeFader::*)(const YADAW::Audio::Device::AudioProcessData<float>&);
public:
    VolumeFader(YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0);
    VolumeFader(const VolumeFader&) = default;
    VolumeFader(VolumeFader&&) noexcept = default;
    VolumeFader& operator=(const VolumeFader&) = default;
    VolumeFader& operator=(VolumeFader&&) noexcept = default;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
public:
    void setVolumeValueSequence(YADAW::Audio::Base::Automation::Value& value);
    void unsetVolumeValueSequence();
private:
    YADAW::Audio::Util::AudioChannelGroup input_;
    YADAW::Audio::Util::AudioChannelGroup output_;
    YADAW::Audio::Base::Automation::Value* valueSequence_ = nullptr;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_VOLUMEFADER
