#ifndef YADAW_SRC_AUDIO_UTIL_CHANNELGROUPCONVERTER
#define YADAW_SRC_AUDIO_UTIL_CHANNELGROUPCONVERTER

#include "audio/util/AudioChannelGroup.hpp"
#include "audio/device/IAudioDevice.hpp"

namespace YADAW::Audio::Util
{
using YADAW::Audio::Device::IAudioChannelGroup;
class ChannelGroupConverter: public YADAW::Audio::Device::IAudioDevice
{
public:
    ChannelGroupConverter(const IAudioChannelGroup& inputChannelGroup,
        const IAudioChannelGroup& outputChannelGroup);
    ~ChannelGroupConverter() noexcept;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
public:
    float getGain(std::uint32_t from, std::uint32_t to) const;
    void setGain(std::uint32_t from, std::uint32_t to, float gain);
private:
    AudioChannelGroup inputChannelGroup_;
    AudioChannelGroup outputChannelGroup_;
    std::vector<std::vector<float>> gain_;
};
}

#endif //YADAW_SRC_AUDIO_UTIL_CHANNELGROUPCONVERTER
