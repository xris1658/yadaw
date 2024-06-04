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
#include "audio/util/Mute.hpp"
#include "audio/util/Summing.hpp"
#include "util/AutoIncrementID.hpp"
#include "util/OptionalUtil.hpp"

// Since this is included by `AudioIOPosition.hpp`, we could not simply include
// <QColor>, otherwise the header could not be located.
// Since we only use `Mixer::Position` in `AudioIOPosition`, it might be a good
// idea to move those definitions into separate files.
#include <QtGui/QColor>

namespace YADAW::Audio::Mixer
{
// Struct of a channel:
// +------------+   +-------------------+   +------+   +-------+   +--------------------+   +-------+   +-------------+
// | Input Node |-->| Pre-Fader Inserts |-->| Mute |-->| Fader |-->| Post-Fader Inserts |-->| Meter |-->| Output Node |
// +------------+   +-------------------+   +------+   +-------+   +--------------------+   +-------+   +-------------+
//
// `Mixer` owns faders, meters and input/output devices of regular channels.
// Input devices:
// - `AudioHardwareInput`: A not-owned `YADAW::Audio::Device::IBus`.
// - `Audio`: An owned `BlankGenerator` if no audio input is selected,
//            a not-owned mute/solo of an audio input channel otherwise.
// - `Instrument`: An owned `IAudioDevice`.
// - `AudioFX`: An owned `YADAW::Audio::Util::Summing`.
// - `AudioBus`: An owned `YADAW::Audio::Util::Summing`.
// - `AudioHardwareOutput`: An owned `YADAW::Audio::Util::Summing`.
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
    using IDGen = YADAW::Util::AutoIncrementID;
    struct Position
    {
        enum Type: std::uint32_t
        {
            Invalid,
            AudioHardwareIOChannel,
            BusAndFXChannel,
            SidechainOfPlugin
        };
        Type type = Type::Invalid;
        std::uint32_t channelGroupIndex = 0;
        IDGen::ID id = IDGen::InvalidId;
        friend bool operator==(const Position& lhs, const Position& rhs)
        {
            return std::tie(lhs.type, lhs.channelGroupIndex, lhs.id)
                == std::tie(rhs.type, rhs.channelGroupIndex, rhs.id);
        }
        friend bool operator!=(const Position& lhs, const Position& rhs)
        {
            return !(lhs == rhs);
        }
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
    OptionalRef<const VolumeFader> audioInputVolumeFaderAt(std::uint32_t index) const;
    OptionalRef<const VolumeFader> audioOutputVolumeFaderAt(std::uint32_t index) const;
    OptionalRef<const VolumeFader> volumeFaderAt(std::uint32_t index) const;
    OptionalRef<VolumeFader> audioInputVolumeFaderAt(std::uint32_t index);
    OptionalRef<VolumeFader> audioOutputVolumeFaderAt(std::uint32_t index);
    OptionalRef<VolumeFader> volumeFaderAt(std::uint32_t index);
    OptionalRef<const YADAW::Audio::Util::Mute> audioInputMuteAt(std::uint32_t index) const;
    OptionalRef<const YADAW::Audio::Util::Mute> audioOutputMuteAt(std::uint32_t index) const;
    OptionalRef<const YADAW::Audio::Util::Mute> muteAt(std::uint32_t index) const;
    OptionalRef<YADAW::Audio::Util::Mute> audioInputMuteAt(std::uint32_t index);
    OptionalRef<YADAW::Audio::Util::Mute> audioOutputMuteAt(std::uint32_t index);
    OptionalRef<YADAW::Audio::Util::Mute> muteAt(std::uint32_t index);
    OptionalRef<const Meter> audioInputMeterAt(std::uint32_t index) const;
    OptionalRef<const Meter> audioOutputMeterAt(std::uint32_t index) const;
    OptionalRef<const Meter> meterAt(std::uint32_t index) const;
    OptionalRef<Meter> audioInputMeterAt(std::uint32_t index);
    OptionalRef<Meter> audioOutputMeterAt(std::uint32_t index);
    OptionalRef<Meter> meterAt(std::uint32_t index);
    std::optional<YADAW::Audio::Base::ChannelGroupType> audioInputChannelGroupTypeAt(std::uint32_t index) const;
    std::optional<YADAW::Audio::Base::ChannelGroupType> audioOutputChannelGroupTypeAt(std::uint32_t index) const;
    std::optional<YADAW::Audio::Base::ChannelGroupType> channelGroupTypeAt(std::uint32_t index) const;
    std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>> audioInputChannelGroupTypeAndChannelCountAt(std::uint32_t index) const;
    std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>> audioOutputChannelGroupTypeAndChannelCountAt(std::uint32_t index) const;
    std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>> channelGroupTypeAndChannelCountAt(std::uint32_t index) const;
    std::optional<IDGen::ID> audioInputChannelIDAt(std::uint32_t index) const;
    std::optional<IDGen::ID> audioOutputChannelIDAt(std::uint32_t index) const;
    std::optional<IDGen::ID> channelIDAt(std::uint32_t index) const;
    OptionalRef<const Position> mainOutputAt(std::uint32_t index) const;
    bool setMainOutputAt(std::uint32_t index, Position position);
    std::optional<std::uint32_t> getInputIndexOfId(IDGen::ID id) const;
    std::optional<std::uint32_t> getOutputIndexOfId(IDGen::ID id) const;
    std::optional<std::uint32_t> getIndexOfId(IDGen::ID id) const;
public:
    bool appendAudioInputChannel(
        const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex);
    bool insertAudioInputChannel(
        std::uint32_t position, const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex);
    bool removeAudioInputChannel(std::uint32_t position, std::uint32_t removeCount = 1);
    void clearAudioInputChannels();
    bool appendAudioOutputChannel(
        const ade::NodeHandle& outNode, std::uint32_t channelGroupIndex);
    bool insertAudioOutputChannel(
        std::uint32_t position, const ade::NodeHandle& outNode, std::uint32_t channel);
    bool removeAudioOutputChannel(std::uint32_t position, std::uint32_t removeCount = 1);
    void clearAudioOutputChannels();
    bool insertChannel(
        std::uint32_t position,
        ChannelType channelType,
        YADAW::Audio::Base::ChannelGroupType chanelGroupType,
        std::uint32_t channelCountInGroup = 0
    );
    bool appendChannel(
        ChannelType channelType,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0
    );
    bool removeChannel(
        std::uint32_t position,
        std::uint32_t removeCount = 1
    );
    void clearChannels();
public:
    void setNodeAddedCallback(NodeAddedCallback* callback);
    void setNodeRemovedCallback(NodeRemovedCallback* callback);
    void setConnectionUpdatedCallback(ConnectionUpdatedCallback* callback);
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
    using MuteAndNode = std::pair<
        std::unique_ptr<YADAW::Audio::Util::Mute>,
        ade::NodeHandle
    >;
    using DeviceAndNode = std::pair<
        std::unique_ptr<YADAW::Audio::Device::IAudioDevice>,
        ade::NodeHandle
    >;
public:
    struct IDAndIndex
    {
        IDAndIndex(IDGen::ID id, std::uint32_t index) : id(id), index(index) {}
        IDAndIndex(const IDAndIndex&) = default;
        IDAndIndex& operator=(IDAndIndex rhs)
        {
            std::swap(id, rhs.id);
            std::swap(index, rhs.index);
            return *this;
        }
        ~IDAndIndex() noexcept = default;
        IDGen::ID id;
        std::uint32_t index;
    };
private:
    YADAW::Audio::Engine::AudioDeviceGraph<
        YADAW::Audio::Engine::Extension::Buffer,
        YADAW::Audio::Engine::Extension::UpstreamLatency> graph_;
    YADAW::Audio::Engine::AudioDeviceGraphWithPDC graphWithPDC_;

    IDGen audioInputChannelIdGen_;
    std::vector<IDGen::ID> audioInputChannelId_;
    std::vector<IDAndIndex> audioInputChannelIdAndIndex_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioInputPreFaderInserts_;
    std::vector<MuteAndNode> audioInputMutes_;
    std::vector<FaderAndNode> audioInputFaders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioInputPostFaderInserts_;
    std::vector<MeterAndNode> audioInputMeters_;
    std::vector<ChannelInfo> audioInputChannelInfo_;

    IDGen channelIdGen_;
    std::vector<IDGen::ID> channelId_;
    std::vector<IDAndIndex> channelIdAndIndex_;
    std::vector<DeviceAndNode> inputDevices_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> preFaderInserts_;
    std::vector<MuteAndNode> mutes_;
    std::vector<FaderAndNode> faders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> postFaderInserts_;
    std::vector<MeterAndNode> meters_;
    std::vector<ChannelInfo> channelInfo_;
    std::vector<Position> mainOutput_;

    IDGen audioOutputChannelIdGen_;
    std::vector<IDGen::ID> audioOutputChannelId_;
    std::vector<IDAndIndex> audioOutputChannelIdAndIndex_;
    std::vector<SummingAndNode> audioOutputSummings_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioOutputPreFaderInserts_;
    std::vector<MuteAndNode> audioOutputMutes_;
    std::vector<FaderAndNode> audioOutputFaders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioOutputPostFaderInserts_;
    std::vector<MeterAndNode> audioOutputMeters_;
    std::vector<ChannelInfo> audioOutputChannelInfo_;

    std::unordered_set<ade::EdgeHandle, ade::HandleHasher<ade::Edge>> connections_;

    NodeAddedCallback* nodeAddedCallback_;
    NodeRemovedCallback* nodeRemovedCallback_;
    ConnectionUpdatedCallback* connectionUpdatedCallback_;
};
}

#endif //YADAW_SRC_AUDIO_MIXER_MIXER
