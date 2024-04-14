#ifndef YADAW_SRC_AUDIO_UTIL_MUTE
#define YADAW_SRC_AUDIO_UTIL_MUTE

#include "audio/base/Automation.hpp"
#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"
#include "util/AtomicMutex.hpp"

namespace YADAW::Audio::Util
{
class Mute: public YADAW::Audio::Device::IAudioDevice
{
private:
    using ProcessFunc = void(Mute::*)(
        const YADAW::Audio::Device::AudioProcessData<float>&,
        const YADAW::Audio::Base::Automation*);
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
    void setProcessStartTime(YADAW::Audio::Base::Automation::Time time);
private:
    void processWithoutAutomation(
        const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
        const YADAW::Audio::Base::Automation* automation);
    void processWithAutomation(
        const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData,
        const YADAW::Audio::Base::Automation* automation);
private:
    static constexpr ProcessFunc processFuncs[2] = {
        &Mute::processWithoutAutomation,
        &Mute::processWithAutomation,
    };
    std::atomic<const YADAW::Audio::Base::Automation*> muteAutomation_ = nullptr;
    YADAW::Audio::Base::Automation::Time time_;
    YADAW::Audio::Util::AudioChannelGroup input_;
    YADAW::Audio::Util::AudioChannelGroup output_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_MUTE
