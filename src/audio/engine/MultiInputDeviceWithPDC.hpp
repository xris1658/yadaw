#ifndef YADAW_SRC_AUDIO_ENGINE_MULTIINPUTDEVICEWITHPDC
#define YADAW_SRC_AUDIO_ENGINE_MULTIINPUTDEVICEWITHPDC

#include "audio/device/IAudioDevice.hpp"
#include "audio/engine/AudioDeviceProcess.hpp"
#include "audio/util/AudioBufferPool.hpp"
#include "audio/util/AudioProcessDataPointerContainer.hpp"
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
    MultiInputDeviceWithPDC(Self&&) noexcept = delete;
    ~MultiInputDeviceWithPDC() override;
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    // The maximum latency introduced by PDCs is not included. If you need the latency info of PDCs,
    // use `getDelayOfPDC` and `getPDCIndexOfMaximumDelay` instead.
    // Ideally, this class is used for compensating the upstream latencies, in which case it does
    // not introduce extra latencies. Use cases of this class without any other facilities that does
    // latency compensation is certainly a terrible idea.
    std::uint32_t latencyInSamples() const override;
    void process(const AudioProcessData<float> &audioProcessData) override;
public:
    void setBufferSize(std::uint32_t newBufferSize);
    const YADAW::Audio::Engine::AudioDeviceProcess process() const;
    YADAW::Audio::Engine::AudioDeviceProcess process();
    std::optional<std::uint32_t> getDelayOfPDC(std::uint32_t audioInputGroupIndex) const;
    std::optional<std::uint32_t> getPDCIndexOfMaximumDelay() const;
    void setDelayOfPDC(std::uint32_t audioInputGroupIndex, std::uint32_t delay);
private:
    YADAW::Audio::Engine::AudioDeviceProcess process_;
    std::vector<YADAW::Audio::Util::SampleDelay> pdcs_;
    std::vector<YADAW::Audio::Util::AudioProcessDataPointerContainer<float>> intermediateContainers_;
    YADAW::Util::IntrusivePointer<YADAW::Audio::Util::AudioBufferPool> pool_;
    std::vector<std::vector<YADAW::Audio::Util::Buffer>> intermediateBuffers_;
    YADAW::Audio::Util::AudioProcessDataPointerContainer<float> deviceProcessDataContainer_;
};
}

#endif //YADAW_SRC_AUDIO_ENGINE_MULTIINPUTDEVICEWITHPDC
