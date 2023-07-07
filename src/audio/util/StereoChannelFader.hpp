#ifndef YADAW_SRC_AUDIO_UTIL_STEREOCHANNELFADER
#define YADAW_SRC_AUDIO_UTIL_STEREOCHANNELFADER

#include "audio/base/StereoPanning.hpp"
#include "audio/device/IAudioDevice.hpp"

#include <bitset>

namespace YADAW::Audio::Util
{
class StereoChannelFader: public YADAW::Audio::Device::IAudioDevice
{
private:
    class AudioChannelGroup: public YADAW::Audio::Device::IAudioChannelGroup
    {
    private:
        static constexpr YADAW::Audio::Base::ChannelGroupType channelGroupType();
    public:
        QString name() const override;
        std::uint32_t channelCount() const override;
        YADAW::Audio::Base::ChannelGroupType type() const override;
        YADAW::Audio::Base::ChannelType speakerAt(std::uint32_t index) const override;
        QString speakerNameAt(std::uint32_t index) const override;
        bool isMain() const override;
    public:
        void setName(const QString& name);
        void setName(QString&& name);
    private:
        QString name_;
    };
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
