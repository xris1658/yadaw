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
    float scale() const;
    void setScale(float scale);
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
private:
    void processWithoutAutomation(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData);
    void processWithAutomation(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData);
public:
    void setAutomation(const YADAW::Audio::Base::Automation& automation,
        YADAW::Audio::Base::Automation::Time time);
    void unsetAutomation(const YADAW::Audio::Base::Automation& automation);
private:
    static constexpr ProcessFunc processFuncs[2] = {
        &VolumeFader::processWithoutAutomation,
        &VolumeFader::processWithAutomation,
    };
    float scale_ = 1.0f;
    YADAW::Audio::Util::AudioChannelGroup input_;
    YADAW::Audio::Util::AudioChannelGroup output_;
    const YADAW::Audio::Base::Automation* automation_ = nullptr;
    YADAW::Audio::Base::Automation::Time time_ = 0;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_VOLUMEFADER
