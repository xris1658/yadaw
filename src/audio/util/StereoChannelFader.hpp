#ifndef YADAW_SRC_AUDIO_UTIL_STEREOCHANNELFADER
#define YADAW_SRC_AUDIO_UTIL_STEREOCHANNELFADER

#include "audio/base/StereoPanning.hpp"
#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

namespace YADAW::Audio::Util
{
class StereoChannelFader: public YADAW::Audio::Device::IAudioDevice
{
public:
    StereoChannelFader();
    ~StereoChannelFader();
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
private:
    void setGain(float gain);
    void setPanning(float panning);
private:
    float gain_ = 1.0;
    float panning_ = 0.0;
    AudioChannelGroup inputAudioChannelGroup_;
    AudioChannelGroup outputAudioChannelGroup_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_STEREOCHANNELFADER
