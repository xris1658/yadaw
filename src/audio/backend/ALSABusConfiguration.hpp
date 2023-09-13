#ifndef YADAW_SRC_AUDIO_BACKEND_ALSABUSCONFIGURATION
#define YADAW_SRC_AUDIO_BACKEND_ALSABUSCONFIGURATION

#if __linux__

#include "audio/backend/ALSABackend.hpp"
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
class ALSABusConfiguration: public YADAW::Audio::Device::IAudioBusConfiguration
{
public:
    class Bus: public YADAW::Audio::Device::IBus, public YADAW::Audio::Device::IAudioDevice
    {
    public:
        Bus(bool isInput, std::uint32_t channelCount);
    public:
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
        void setAudioBuffers(
            const YADAW::Audio::Backend::ALSABackend::AudioBuffer* buffers);
    private:
        std::vector<Channel> channels_;
        const YADAW::Audio::Backend::ALSABackend::AudioBuffer* buffers_;
        bool isInput_;
        YADAW::Audio::Util::AudioChannelGroup audioChannelGroup_;
    };
public:
    ALSABusConfiguration(const ALSABackend& backend);
public:
    std::uint32_t inputBusCount() const override;
    std::uint32_t outputBusCount() const override;
    OptionalRef<const IBus> inputBusAt(std::uint32_t index) const override;
    OptionalRef<const IBus> outputBusAt(std::uint32_t index) const override;
    OptionalRef<IBus> inputBusAt(std::uint32_t index) override;
    OptionalRef<IBus> outputBusAt(std::uint32_t index) override;
    ChannelPosition channelPosition(bool isInput, Channel channel) const override;
    uint32_t appendBus(bool isInput, std::uint32_t channelCount) override;
    bool removeBus(bool isInput, std::uint32_t index) override;
    void clearBus(bool isInput) override;
private:
    const ALSABackend* backend_;
    std::vector<std::unique_ptr<Bus>> inputBusses_;
    std::vector<std::unique_ptr<Bus>> outputBusses_;
};
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_ALSABUSCONFIGURATION
