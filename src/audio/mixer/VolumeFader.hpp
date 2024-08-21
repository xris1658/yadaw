#ifndef YADAW_SRC_AUDIO_MIXER_VOLUMEFADER
#define YADAW_SRC_AUDIO_MIXER_VOLUMEFADER

#include "audio/base/Automation.hpp"
#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"

namespace YADAW::Audio::Mixer
{
class VolumeFader: public YADAW::Audio::Device::IAudioDevice,
    public YADAW::Audio::Device::IAudioDeviceParameter
{
private:
    using ProcessFunc = void(VolumeFader::*)(const YADAW::Audio::Device::AudioProcessData<float>&);
public:
    enum ParamId: std::uint32_t
    {
        Gain = 0
    };
public:
    class GainParameter: public YADAW::Audio::Device::IParameter
    {
        friend class VolumeFader;
    private:
        GainParameter(VolumeFader& volumeFader);
    public:
        ~GainParameter();
    public:
        std::uint32_t id() const override;
        QString name() const override;
        double minValue() const override;
        double maxValue() const override;
        double defaultValue() const override;
        double value() const override;

        double stepSize() const override;
        std::uint32_t stepCount() const override;
        QString unit() const override;
    public:
        QString valueToString(double value) const override;
        double stringToValue(const QString& string) const override;
    private:
        VolumeFader& volumeFader_;
    };
    friend class GainParameter;
public:
    VolumeFader(YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0);
    VolumeFader(const VolumeFader&) = default;
    VolumeFader(VolumeFader&&) noexcept = default;
    VolumeFader& operator=(const VolumeFader&) = default;
    VolumeFader& operator=(VolumeFader&&) noexcept = default;
public:
    bool initialize(double sampleRate, std::uint32_t maxSampleCount);
    void uninitialize();
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;
public:
    std::uint32_t parameterCount() const override;
    YADAW::Audio::Device::IParameter* parameter(std::uint32_t index) override;
    const YADAW::Audio::Device::IParameter* parameter(std::uint32_t index) const override;
public:
    void beginEditGain();
    void performEditGain(double value);
    void endEditGain();
    void onBufferSwitched(std::int64_t switchTimestampInNanosecond);
private:
    void addPoint();
    void addPoint(double value);
private:
    YADAW::Audio::Util::AudioChannelGroup input_;
    YADAW::Audio::Util::AudioChannelGroup output_;
    GainParameter gainParameter_;
    std::vector<std::pair<std::uint32_t, double>> valuePoints_[2];
    double sampleRate_ = 0.0;
    double prevValue_ = 0.0;
    std::int64_t switchTimestampInNanosecond_ = 0;
    std::vector<double> valueSeq_[2];
};
}

#endif // YADAW_SRC_AUDIO_MIXER_VOLUMEFADER
