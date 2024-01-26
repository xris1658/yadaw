#ifndef YADAW_SRC_AUDIO_UTIL_SUMMING
#define YADAW_SRC_AUDIO_UTIL_SUMMING

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

#include <vector>

namespace YADAW::Audio::Util
{
class Summing: public YADAW::Audio::Device::IAudioDevice
{
public:
    Summing(std::uint32_t inputCount, YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0);
    Summing(const Summing& rhs) = default;
    Summing(Summing&& rhs) noexcept = default;
    Summing& operator=(const Summing& rhs) = default;
    Summing& operator=(Summing&& rhs) noexcept = default;
    ~Summing() noexcept = default;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const Device::AudioProcessData<float>& audioProcessData) override;
    std::uint32_t audioChannelMapCount() const override;
    OptionalChannelMap audioChannelMapAt(std::uint32_t index) const override;
private:
     std::uint32_t inputCount_;
    YADAW::Audio::Util::AudioChannelGroup inputs_;
    YADAW::Audio::Util::AudioChannelGroup output_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_SUMMING
