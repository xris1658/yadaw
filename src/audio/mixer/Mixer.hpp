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
        AudioBus,
        AudioHardwareInput,
        AudioHardwareOutput
    };
    struct ChannelInfo
    {
        QString name;
        QColor color;
        ChannelType channelType;
    };
    using NodeAddedCallback = void(const Mixer&);
    using NodeRemovedCallback = void(const Mixer&);
    using ConnectionUpdatedCallback = void(const Mixer&);
public:
    Mixer();
public:
    const YADAW::Audio::Engine::AudioDeviceGraphWithPDC& graph() const;
    YADAW::Audio::Engine::AudioDeviceGraphWithPDC& graph();
    const YADAW::Audio::Engine::Extension::Buffer& bufferExtension() const;
    YADAW::Audio::Engine::Extension::Buffer& bufferExtension();
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
    OptionalRef<const ChannelInfo> audioInputChannelInfoAt(std::uint32_t index) const;
    OptionalRef<const ChannelInfo> audioOutputChannelInfoAt(std::uint32_t index) const;
    OptionalRef<const ChannelInfo> channelInfoAt(std::uint32_t index) const;
    OptionalRef<ChannelInfo> audioInputChannelInfoAt(std::uint32_t index);
    OptionalRef<ChannelInfo> audioOutputChannelInfoAt(std::uint32_t index);
    OptionalRef<ChannelInfo> channelInfoAt(std::uint32_t index);
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
        std::uint32_t channelCountInGroup = 0
    );
    bool appendChannel(
        ChannelType channelType,
        YADAW::Audio::Base::ChannelGroupType channelGroupType = YADAW::Audio::Base::ChannelGroupType::eInvalid,
        std::uint32_t channelCountInGroup = 0
    );
    bool removeChannel(
        std::uint32_t position,
        std::uint32_t removeCount = 1
    );
    void clearChannels();
public:
    void setNodeAddedCallback(std::function<NodeAddedCallback>&& callback);
    void setNodeRemovedCallback(std::function<NodeRemovedCallback>&& callback);
    void setConnectionUpdatedCallback(std::function<ConnectionUpdatedCallback>&& callback);
    void resetNodeAddedCallback();
    void resetNodeRemovedCallback();
    void resetConnectionUpdatedCallback();
public:
    std::optional<ade::EdgeHandle> addConnection(
        const ade::NodeHandle& from, const ade::NodeHandle& to,
        std::uint32_t fromChannel, std::uint32_t toChannel);
    void removeConnection(
        const ade::EdgeHandle& edgeHandle
    );
    void clearConnections();
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
    using DeviceAndNode = std::pair<
        std::unique_ptr<YADAW::Audio::Device::IAudioDevice>,
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
    std::vector<ChannelInfo> audioInputChannelInfo_;

    std::vector<DeviceAndNode> inputDevices_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> preFaderInserts_;
    std::vector<FaderAndNode> faders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> postFaderInserts_;
    std::vector<MeterAndNode> meters_;
    std::vector<DeviceAndNode> outputDevices_;
    std::vector<ChannelInfo> channelInfo_;

    std::vector<SummingAndNode> audioOutputSummings_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioOutputPreFaderInserts_;
    std::vector<FaderAndNode> audioOutputFaders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioOutputPostFaderInserts_;
    std::vector<MeterAndNode> audioOutputMeters_;
    std::vector<ChannelInfo> audioOutputChannelInfo_;

    std::unordered_set<ade::EdgeHandle, ade::HandleHasher<ade::Edge>> connections_;

    std::function<NodeAddedCallback> nodeAddedCallback_;
    std::function<NodeRemovedCallback> nodeRemovedCallback_;
    std::function<ConnectionUpdatedCallback> connectionUpdatedCallback_;
};
}

#endif //YADAW_SRC_AUDIO_MIXER_MIXER
