#ifndef YADAW_TEST_COMMON_SINEWAVEGENERATOR
#define YADAW_TEST_COMMON_SINEWAVEGENERATOR

#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"
#include "util/IntegerRange.hpp"

class SineWaveGenerator: public YADAW::Audio::Device::IAudioDevice
{
public:
    SineWaveGenerator(YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0);
    SineWaveGenerator(const SineWaveGenerator&) = default;
    SineWaveGenerator(SineWaveGenerator&&) noexcept = default;
    SineWaveGenerator& operator=(const SineWaveGenerator&) = default;
    SineWaveGenerator& operator=(SineWaveGenerator&&) = default;
    ~SineWaveGenerator() noexcept = default;
public:
    double frequency() const;
    double sampleRate() const;
    bool setFrequency(double frequency);
    bool setSampleRate(double sampleRate);
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
    std::uint32_t audioChannelMapCount() const override;
    OptionalChannelMap audioChannelMapAt(std::uint32_t index) const override;
private:
    bool processing_;
    double frequency_;
    double sampleRate_;
    std::uint64_t frameCount_;
    YADAW::Audio::Util::AudioChannelGroup channelGroup_;
};

#endif //YADAW_TEST_COMMON_SINEWAVEGENERATOR
