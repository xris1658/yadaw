#ifndef YADAW_SRC_AUDIO_ENGINE_MULTIINPUTDEVICEWITHPDC
#define YADAW_SRC_AUDIO_ENGINE_MULTIINPUTDEVICEWITHPDC

#include "audio/device/IAudioDevice.hpp"
#include "audio/engine/AudioDeviceProcess.hpp"
#include "audio/util/SampleDelay.hpp"

namespace YADAW::Audio::Engine
{
class MultiInputDeviceWithPDC:
    public YADAW::Audio::Device::IAudioDevice
{
    using Self = MultiInputDeviceWithPDC;
public:
    MultiInputDeviceWithPDC(YADAW::Audio::Engine::AudioDeviceProcess&& process);
    MultiInputDeviceWithPDC(const Self&) = delete;
    MultiInputDeviceWithPDC(Self&&) = delete;
    ~MultiInputDeviceWithPDC() override;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
public:
    void process(const AudioProcessData<float> &audioProcessData) override;
public:
    void setDelay(std::uint32_t audioInputGroupIndex, std::uint32_t delay);
private:
    YADAW::Audio::Engine::AudioDeviceProcess process_;
    std::vector<YADAW::Audio::Util::SampleDelay> pdcs_;
    std::vector<YADAW::Audio::Device::AudioProcessData<float>> pdcAudioProcessData_;
    std::vector<std::uint32_t> upstreamLatencies_;
};
}

#endif //YADAW_SRC_AUDIO_ENGINE_MULTIINPUTDEVICEWITHPDC
