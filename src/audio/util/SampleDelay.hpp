#ifndef YADAW_SRC_AUDIO_UTIL_SAMPLEDELAY
#define YADAW_SRC_AUDIO_UTIL_SAMPLEDELAY

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"
#include "util/CircularDeque.hpp"

#include <vector>

namespace YADAW::Audio::Util
{
class SampleDelay: public YADAW::Audio::Device::IAudioDevice
{
public:
    SampleDelay(std::uint32_t delay,
        const YADAW::Audio::Device::IAudioChannelGroup& channelGroup
    );
    SampleDelay(const SampleDelay&) = delete;
    SampleDelay(SampleDelay&& rhs) noexcept;
    SampleDelay& operator=(const SampleDelay&) = delete;
    SampleDelay& operator=(SampleDelay&& rhs) noexcept;
    ~SampleDelay() noexcept;
public:
    bool setDelay(std::uint32_t delay);
    bool isProcessing() const;
    bool startProcessing();
    bool stopProcessing();
public:
    std::uint32_t audioInputGroupCount() const override;
    std::uint32_t audioOutputGroupCount() const override;
    OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
    OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
    std::uint32_t latencyInSamples() const override;
    void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;

private:
    void doProcessIfDelayIsZero(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData);
    void doProcessIfDelayIsNotZero(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData);
private:
    std::uint32_t delay_;
    bool processing_;
    void (SampleDelay::*processFunc_)(const YADAW::Audio::Device::AudioProcessData<float>&);
    std::uint64_t offset_;
    std::vector<std::vector<float>> buffers_;
    AudioChannelGroup channelGroup_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_SAMPLEDELAY
