#ifndef YADAW_SRC_AUDIO_BACKEND_AUDIOGRAPHBUSCONFIGURATION
#define YADAW_SRC_AUDIO_BACKEND_AUDIOGRAPHBUSCONFIGURATION

#if _WIN32

#include "audio/backend/AudioGraphBackend.hpp"
#include "audio/base/Channel.hpp"
#include "audio/device/IAudioBusConfiguration.hpp"
#include "audio/device/IAudioDevice.hpp"
#include "audio/util/AudioChannelGroup.hpp"
#include "util/OptionalUtil.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace YADAW::Audio::Backend
{
using YADAW::Audio::Device::Channel;
using YADAW::Audio::Device::IBus;
class AudioGraphBusConfiguration: public YADAW::Audio::Device::IAudioBusConfiguration
{
public:
    class Bus: public YADAW::Audio::Device::IBus
    {
    public:
        Bus(bool isInput, std::uint32_t channelCount);
        Bus(bool isInput, YADAW::Audio::Base::ChannelGroupType channelGroupType,
            std::uint32_t channelCount = 0);
    public: // IBus interfaces
        std::optional<Channel> channelAt(std::uint32_t index) const override;
        bool setChannel(std::uint32_t index, Channel channel) override;
    public: // IAudioDevice interfaces
        std::uint32_t audioInputGroupCount() const override;
        std::uint32_t audioOutputGroupCount() const override;
        OptionalAudioChannelGroup audioInputGroupAt(std::uint32_t index) const override;
        OptionalAudioChannelGroup audioOutputGroupAt(std::uint32_t index) const override;
        std::uint32_t latencyInSamples() const override;
        void process(const YADAW::Audio::Device::AudioProcessData<float>& audioProcessData) override;

    public:
        void setName(const QString& name);
        void setName(QString&& name);
        void setInterleaveAudioBuffers(
            const YADAW::Audio::Backend::AudioGraphBackend::InterleaveAudioBuffer* buffers);
    private:
        std::vector<Channel> channels_;
        const YADAW::Audio::Backend::AudioGraphBackend::InterleaveAudioBuffer* buffers_;
        bool isInput_;
        YADAW::Audio::Util::AudioChannelGroup audioChannelGroup_;
    };
public:
    AudioGraphBusConfiguration(const AudioGraphBackend& backend);
public:
    std::uint32_t inputBusCount() const override;
    std::uint32_t outputBusCount() const override;
    OptionalRef<const IBus> inputBusAt(std::uint32_t index) const override;
    OptionalRef<const IBus> outputBusAt(std::uint32_t index) const override;
    OptionalRef<IBus> inputBusAt(std::uint32_t index) override;
    OptionalRef<IBus> outputBusAt(std::uint32_t index) override;
    ChannelPosition channelPosition(bool isInput, Channel channel) const override;
    std::uint32_t appendBus(bool isInput,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCount = 0) override;
    bool removeBus(bool isInput, std::uint32_t index) override;
    void clearBus(bool isInput) override;
public:
    OptionalRef<const Bus> getInputBusAt(std::uint32_t index) const;
    OptionalRef<const Bus> getOutputBusAt(std::uint32_t index) const;
    OptionalRef<Bus> getInputBusAt(std::uint32_t index);
    OptionalRef<Bus> getOutputBusAt(std::uint32_t index);
public:
    void setBuffers(const AudioGraphBackend::InterleaveAudioBuffer* inputs,
        const AudioGraphBackend::InterleaveAudioBuffer* outputs);
private:
    const AudioGraphBackend* backend_;
    std::vector<std::unique_ptr<Bus>> inputBusses_;
    std::vector<std::unique_ptr<Bus>> outputBusses_;
};
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_AUDIOGRAPHBUSCONFIGURATION
