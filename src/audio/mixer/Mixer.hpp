#ifndef YADAW_SRC_AUDIO_MIXER_MIXER
#define YADAW_SRC_AUDIO_MIXER_MIXER

#include "audio/base/Channel.hpp"
#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioDeviceGraphWithPDC.hpp"
#include "audio/engine/extension/Buffer.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/mixer/Inserts.hpp"
#include "audio/mixer/Meter.hpp"
#include "audio/mixer/VolumeFader.hpp"
#include "audio/util/Summing.hpp"
#include "util/OptionalUtil.hpp"

namespace YADAW::Audio::Mixer
{
class Mixer
{
public:
    enum class ChannelType
    {
        Audio,
        Instrument,
        AudioFX,
        Group
    };
public:
    Mixer();
public:
    const YADAW::Audio::Engine::AudioDeviceGraphWithPDC& graph() const;
    YADAW::Audio::Engine::AudioDeviceGraphWithPDC& graph();
    const YADAW::Audio::Engine::Extension::Buffer& buffer() const;
    YADAW::Audio::Engine::Extension::Buffer& buffer();
public:
    std::uint32_t audioInputChannelCount() const;
    std::uint32_t channelCount() const;
    std::uint32_t audioOutputChannelCount() const;
    OptionalRef<const YADAW::Audio::Mixer::Inserts> audioInputChannelPreFaderInsertsAt(std::uint32_t index) const;
    OptionalRef<const YADAW::Audio::Mixer::Inserts> audioInputChannelPostFaderInsertsAt(std::uint32_t index) const;
    OptionalRef<const YADAW::Audio::Mixer::Inserts> channelPreFaderInsertsAt(std::uint32_t index) const;
    OptionalRef<const YADAW::Audio::Mixer::Inserts> channelPostFaderInsertsAt(std::uint32_t index) const;
    OptionalRef<const YADAW::Audio::Mixer::Inserts> audioOutputChannelPreFaderInsertsAt(std::uint32_t index) const;
    OptionalRef<const YADAW::Audio::Mixer::Inserts> audioOutputChannelPostFaderInsertsAt(std::uint32_t index) const;
    OptionalRef<YADAW::Audio::Mixer::Inserts> audioInputChannelPreFaderInsertsAt(std::uint32_t index);
    OptionalRef<YADAW::Audio::Mixer::Inserts> audioInputChannelPostFaderInsertsAt(std::uint32_t index);
    OptionalRef<YADAW::Audio::Mixer::Inserts> channelPreFaderInsertsAt(std::uint32_t index);
    OptionalRef<YADAW::Audio::Mixer::Inserts> channelPostFaderInsertsAt(std::uint32_t index);
    OptionalRef<YADAW::Audio::Mixer::Inserts> audioOutputChannelPreFaderInsertsAt(std::uint32_t index);
    OptionalRef<YADAW::Audio::Mixer::Inserts> audioOutputChannelPostFaderInsertsAt(std::uint32_t index);
public:
    bool appendAudioInputChannel(
        const ade::NodeHandle& inNode, std::uint32_t channel);
    bool removeAudioInputChannel(std::uint32_t position, std::uint32_t removeCount = 1);
    void clearAudioInputChannels();
    bool appendAudioOutputChannel(
        const ade::NodeHandle& outNode, std::uint32_t channel);
    bool removeAudioOutputChannel(std::uint32_t position, std::uint32_t removeCount = 1);
    void clearAudioOutputChannels();
    bool insertChannel(
        std::uint32_t position,
        ChannelType channelType,
        YADAW::Audio::Base::ChannelGroupType chanelGroupType = YADAW::Audio::Base::ChannelGroupType::eInvalid,
        std::uint32_t channelCount = 0
    );
    bool appendChannel(
        ChannelType channelType,
        YADAW::Audio::Base::ChannelGroupType channelGroupType = YADAW::Audio::Base::ChannelGroupType::eInvalid,
        std::uint32_t channelCount = 0
    );
    void removeChannel(
        std::uint32_t position,
        std::uint32_t removeCount = 1
    );
    void clearChannels();
private:
    using MeterAndNode = std::pair<
        std::unique_ptr<YADAW::Audio::Mixer::Meter>,
        ade::NodeHandle
    >;
    using FaderAndNode = std::pair<
        std::unique_ptr<YADAW::Audio::Mixer::VolumeFader>,
        ade::NodeHandle
    >;
    using SummingAndNode = std::pair<
        std::unique_ptr<YADAW::Audio::Util::Summing>,
        ade::NodeHandle
    >;
private:
    YADAW::Audio::Engine::AudioDeviceGraph<
        YADAW::Audio::Engine::Extension::Buffer,
        YADAW::Audio::Engine::Extension::UpstreamLatency> graph_;
    YADAW::Audio::Engine::AudioDeviceGraphWithPDC graphWithPDC_;

    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioInputPreFaderInserts_;
    std::vector<FaderAndNode> audioInputFaders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioInputPostFaderInserts_;
    std::vector<MeterAndNode> audioInputMeters_;

    std::vector<ade::NodeHandle> instruments_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> preFaderInserts_;
    std::vector<FaderAndNode> faders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> postFaderInserts_;
    std::vector<MeterAndNode> meters_;

    std::vector<SummingAndNode> audioOutputSummings_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioOutputPreFaderInserts_;
    std::vector<FaderAndNode> audioOutputFaders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioOutputPostFaderInserts_;
    std::vector<MeterAndNode> audioOutputMeters_;
};
}

#endif //YADAW_SRC_AUDIO_MIXER_MIXER
