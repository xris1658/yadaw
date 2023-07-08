#ifndef YADAW_SRC_AUDIO_UTIL_SPLITTER
#define YADAW_SRC_AUDIO_UTIL_SPLITTER

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

#include <vector>

namespace YADAW::Audio::Util
{
class Splitter: public YADAW::Audio::Device::IAudioDevice
{
private:
    Splitter(std::uint32_t outputCount, YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0);
    Splitter(const Splitter&) = default;
    Splitter(Splitter&&) noexcept = default;
    Splitter& operator=(const Splitter&) = default;
    Splitter& operator=(Splitter&&) noexcept = default;
    ~Splitter() noexcept = default;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const Device::AudioProcessData<float>& audioProcessData) override;
private:
    YADAW::Audio::Util::AudioChannelGroup input_;
    std::vector<YADAW::Audio::Util::AudioChannelGroup> outputs_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_SPLITTER
