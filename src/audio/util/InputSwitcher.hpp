#ifndef YADAW_SRC_AUDIO_UTIL_INPUTSWITCHER
#define YADAW_SRC_AUDIO_UTIL_INPUTSWITCHER

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

#include <atomic>

namespace YADAW::Audio::Util
{
class InputSwitcher: public YADAW::Audio::Device::IAudioDevice
{
public:
    InputSwitcher(std::uint32_t inputCount,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCount = 0);
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const Device::AudioProcessData<float>& audioProcessData) override;
public:
    bool setInputIndex(std::uint32_t inputIndex);
private:
    AudioChannelGroup inputChannelGroup_;
    AudioChannelGroup outputChannelGroup_;
    std::uint32_t inputGroupCount_;
    std::atomic<std::uint32_t> inputIndex_ {0U};
};
}

#endif //YADAW_SRC_AUDIO_UTIL_INPUTSWITCHER
