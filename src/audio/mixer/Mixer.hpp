#ifndef YADAW_SRC_AUDIO_MIXER_MIXER
#define YADAW_SRC_AUDIO_MIXER_MIXER

#include "audio/base/Channel.hpp"
#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/AudioDeviceGraphWithPDC.hpp"
#include "audio/engine/extension/Buffer.hpp"
#include "audio/engine/extension/NameTag.hpp"
#include "audio/engine/extension/UpstreamLatency.hpp"
#include "audio/mixer/Common.hpp"
#include "audio/mixer/Inserts.hpp"
#include "audio/mixer/Meter.hpp"
#include "audio/mixer/PolarityInverter.hpp"
#include "audio/mixer/VolumeFader.hpp"
#include "audio/util/Mute.hpp"
#include "audio/util/Summing.hpp"
#include "util/AutoIncrementID.hpp"
#include "util/BatchUpdater.hpp"
#include "util/OptionalUtil.hpp"

#include <QColor>

#include <cstdint>
#include <concepts>
#include <optional>

namespace YADAW::Audio::Mixer
{
class Mixer;

template<typename Factory, typename Device>
concept DeviceFactory = requires
(
    Factory factory,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    { factory(channelGroupType, channelCountInGroup) }
    -> std::same_as<std::unique_ptr<Device>>;
}
&& std::derived_from<Device, YADAW::Audio::Device::IAudioDevice>;

template<typename Device>
using DeviceFactoryType = std::unique_ptr<Device>(
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup
);

// Struct of a channel:
// +------------+   +-------------------+   +-------------------+   +------+     +-------+     +--------------------+   +-------+   +-------------+
// | Input Node |-->| Polarity Inverter |-->| Pre-Fader Inserts |-->| Mute |--+->| Fader |--+->| Post-Fader Inserts |-->| Meter |-->| Output Node |
// +------------+   +-------------------+   +-------------------+   +------+  |  +-------+  |  +--------------------+   +-------+   +-------------+
//                                                                            |             |
//                                                                            |             |  +------+   +------------------+
//                                                                            |             +->| Send |-->| Destination Node |
//                                                                            |                +------+   +------------------+
//                                                                            |
//                                                                            |  +------+   +------------------+
//                                                                            +->| Send |-->| Destination Node |
//                                                                               +------+   +------------------+
//
// `Mixer` owns faders, meters and input/output devices of regular channels.
// Input devices:
// - `AudioHardwareInput`: A not-owned `YADAW::Audio::Device::IBus`.
// - `Audio`: An `InputSwitcher` with 2 inputs:
//            1. Contents of the audio track. Not connected for now,
//               reserved for future use.
//            2. A not-owned mute/solo of the external audio in, or meter of
//               channels, or send faders.
// - `Instrument`: A not-owned `IAudioDevice` which is the instrument, or null
//                 if there is no instrument.
// - `AudioFX`: An owned `YADAW::Audio::Util::Summing`.
// - `AudioBus`: An owned `YADAW::Audio::Util::Summing`.
// - `AudioHardwareOutput`: An owned `YADAW::Audio::Util::Summing`.
//
// Struct of a send:
// +-------------------+   +------+   +-------+
// | Polarity Inverter |-->| Mute |-->| Fader |
// +-------------------+   +------+   +-------+
// There's a few DAW mixer which has polarity inverters in send controls.
// Polarity inverters in sends are useful in some scenarios, so I ended up
// adding one.
//
// Managing Connections
// It's easy to connect/disconnect things since `Mixer` is a managed graph.
// The complicated part is showing "the other side(s)" on both ends.
// I've made some conventions on managing connections that functions should obey
// for now:
// - Always do the actual connection/disconnection (i.e. calling `connect` and
//   `disconnect`) from the source node side. If the relevant callbacks are set,
//   the source node side is solely responsible for invoking those callbacks.
//   For example:
//   - Call `addAuxOutputDestination` if `setAuxInputSource` to
//     `Position::PluginAuxIO`.
//   - Call `removeSend` if `setAuxInputSource` to `Position::Invalid` and the
//     previous input source is `Position::Send`.
// - When connecting things, always call the callback associated to the source
//   (if any) BEFORE invoking the callback associated to the destination (if any).
//   "Plug the source with a new wire before plugging the destination."
//   For example:
//   - When `setAuxInputSource` to `Position::PluginAuxIO`, call
//     `AuxOutputAddedCallback`, then `AuxInputChangedCallback`.
// - When disconnecting things, always call the callback associated to the source
//   (if any) AFTER invoking the callback associated to the SOURCE (if any).
//   "Unplug the source after unplugging the destination."
//   For example:
//   - When `setAuxInputSource` to `Position::Invalid` and the previous input
//     source is `Position::Send`, call `AuxInputChangedCallback`, then
//     `AuxOutputRemovedCallback`.
// - If setting the destination of an aux output or send, split the entire
//   operation as two steps, the first being disconnecting, and the second being
//   connecting.
//   - Even if some callbacks might seem redundant, DO NOT omit those callbacks!
class Mixer
{
public:
    enum ChannelListType { AudioHardwareInputList, RegularList, AudioHardwareOutputList };
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
    struct Position
    {
        enum Type: std::uint32_t
        {
            Invalid,
            AudioHardwareIOChannel,
            SendAndFXChannel,
            AudioChannel,
            Send,
            PluginAuxIO
        };
        Type type = Type::Invalid;
        IDGen::ID id = IDGen::InvalidId;
        friend bool operator==(const Position& lhs, const Position& rhs)
        {
            return std::tie(lhs.type, lhs.id)
                == std::tie(rhs.type, rhs.id);
        }
        friend bool operator!=(const Position& lhs, const Position& rhs)
        {
            return !(lhs == rhs);
        }
    };
    struct PluginAuxIOPosition
    {
        ChannelListType channelListType;
        std::uint32_t channelIndex;
        enum InChannelPosition { Instrument, Inserts };
        InChannelPosition inChannelPosition;
        bool isPreFaderInsert;
        std::uint32_t insertIndex;
        std::uint32_t channelGroupIndex;
    };
    struct SendPosition
    {
        ChannelListType channelListType;
        std::uint32_t channelIndex;
        std::uint32_t sendIndex;
    };
    using ConnectionUpdatedCallback = void(const Mixer&);
    using PreInsertChannelCallbackArgs = struct { std::uint32_t position; std::uint32_t count; };
    using PreInsertChannelCallback = void(const Mixer& sender, PreInsertChannelCallbackArgs args);
    using SendAddedCallback = void(const Mixer& sender, const SendPosition& sendPosition);
    using SendDestinationChangedCallback = void(const Mixer& sender, const SendPosition& sendPosition);
    struct SendRemovedCallbackArgs
    {
        SendPosition sendPosition;
        std::uint32_t removeCount;
    };
    using SendRemovedCallback = void(const Mixer& sender, SendRemovedCallbackArgs args);
    using AuxInputChangedCallback = void(const Mixer& sender, const PluginAuxIOPosition& auxInput);
    struct AuxOutputAddedCallbackArgs
    {
        PluginAuxIOPosition auxOutput;
        std::uint32_t position;
    };
    using AuxOutputDestinationChangedCallbackArgs = AuxOutputAddedCallbackArgs;
    struct AuxOutputRemovedCallbackArgs
    {
        PluginAuxIOPosition auxOutput;
        std::uint32_t position;
        std::uint32_t removeCount;
    };
    using AuxOutputAddedCallback = void(const Mixer& sender, AuxOutputAddedCallbackArgs args);
    using AuxOutputDestinationChangedCallback = void(const Mixer& sender, AuxOutputDestinationChangedCallbackArgs args);
    using AuxOutputRemovedCallback = void(const Mixer& sender, AuxOutputRemovedCallbackArgs args);
private:
    Mixer();
public:
    template<
        DeviceFactory<VolumeFader> VolumeFaderFactory,
        DeviceFactory<Meter> MeterFactory
    >
    Mixer(
        VolumeFaderFactory&& volumeFaderFactory,
        MeterFactory&& meterFactory):
        Mixer()
    {
        volumeFaderFactory_ = std::move(volumeFaderFactory);
        meterFactory_ = std::move(meterFactory);
    }
public:
    const YADAW::Audio::Engine::AudioDeviceGraphWithPDC& graph() const;
    YADAW::Audio::Engine::AudioDeviceGraphWithPDC& graph();
    const YADAW::Audio::Engine::Extension::Buffer& bufferExtension() const;
    YADAW::Audio::Engine::Extension::Buffer& bufferExtension();
    const YADAW::Audio::Engine::Extension::NameTag& nameTagExtension() const;
    YADAW::Audio::Engine::Extension::NameTag& nameTag();
public: // TODO: Add API with `ChannelListType` to remove redundant codes
    std::uint32_t count(ChannelListType type) const;
    OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> polarityInverterAt(ChannelListType type, std::uint32_t index) const;
    OptionalRef<      YADAW::Audio::Mixer::PolarityInverter> polarityInverterAt(ChannelListType type, std::uint32_t index);
    OptionalRef<const YADAW::Audio::Mixer::Inserts> preFaderInsertsAt(ChannelListType type, std::uint32_t index) const;
    OptionalRef<      YADAW::Audio::Mixer::Inserts> preFaderInsertsAt(ChannelListType type, std::uint32_t index);
    OptionalRef<const YADAW::Audio::Mixer::Inserts> postFaderInsertsAt(ChannelListType type, std::uint32_t index) const;
    OptionalRef<      YADAW::Audio::Mixer::Inserts> postFaderInsertsAt(ChannelListType type, std::uint32_t index);
    std::optional<const std::uint32_t> sendCount(ChannelListType type, std::uint32_t index) const;
    std::optional<const bool> sendIsPreFader(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    std::optional<const Position> sendDestination(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    std::optional<const IDGen::ID> sendID(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<const VolumeFader> sendFaderAt(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<      VolumeFader> sendFaderAt(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<const YADAW::Audio::Util::Mute> sendMuteAt(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<      YADAW::Audio::Util::Mute> sendMuteAt(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<const PolarityInverter> sendPolarityInverterAt(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<      PolarityInverter> sendPolarityInverterAt(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<const ChannelInfo> channelInfoAt(ChannelListType type, std::uint32_t index) const;
    OptionalRef<      ChannelInfo> channelInfoAt(ChannelListType type, std::uint32_t index);
    OptionalRef<const VolumeFader> volumeFaderAt(ChannelListType type, std::uint32_t index) const;
    OptionalRef<      VolumeFader> volumeFaderAt(ChannelListType type, std::uint32_t index);
    OptionalRef<const YADAW::Audio::Util::Mute> muteAt(ChannelListType type, std::uint32_t index) const;
    OptionalRef<      YADAW::Audio::Util::Mute> muteAt(ChannelListType type, std::uint32_t index);
    OptionalRef<const Meter> meterAt(ChannelListType type, std::uint32_t index) const;
    OptionalRef<      Meter> meterAt(ChannelListType type, std::uint32_t index);
    std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>> channelGroupTypeAndChannelCountAt(ChannelListType type, std::uint32_t index) const;
    std::optional<IDGen::ID> idAt(ChannelListType type, std::uint32_t index) const;
    std::optional<std::uint32_t> getChannelIndexOfId(ChannelListType type, IDGen::ID id) const;
    bool hasMute(ChannelListType type) const;
    void unmute(ChannelListType type);
    void unmute();
public:
    bool remove(ChannelListType type, std::uint32_t index, std::uint32_t removeCount = 1);
    void clear(ChannelListType type);
    void clear();
public:
    std::optional<bool> appendSend(ChannelListType type, std::uint32_t channelIndex, bool isPreFader, Position destination);
    std::optional<bool> insertSend(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendPosition, bool isPreFader, Position destination);
    std::optional<bool> setSendPreFader(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex, bool preFader);
    std::optional<bool> setSendDestination(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex, Position destination);
    std::optional<bool> removeSend(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendPosition, std::uint32_t removeCount = 1);
    std::optional<bool> clearSends(ChannelListType type, std::uint32_t channelIndex);
public:
    OptionalRef<const Position> mainInputAt(std::uint32_t index) const;
    bool setMainInputAt(std::uint32_t index, Position position);
    OptionalRef<const Position> mainOutputAt(std::uint32_t index) const;
    bool setMainOutputAt(std::uint32_t index, Position position);
public:
    bool appendAudioInputChannel(
        const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex);
    bool insertAudioInputChannel(
        std::uint32_t position, const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex);
    bool appendAudioOutputChannel(
        const ade::NodeHandle& outNode, std::uint32_t channelGroupIndex);
    bool insertAudioOutputChannel(
        std::uint32_t position, const ade::NodeHandle& outNode, std::uint32_t channel);
    bool insertChannels(
        std::uint32_t position, std::uint32_t count,
        ChannelType channelType,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0
    );
    bool appendChannels(
        std::uint32_t count,
        ChannelType channelType,
        YADAW::Audio::Base::ChannelGroupType channelGroupType,
        std::uint32_t channelCountInGroup = 0
    );
public:
    ade::NodeHandle getInstrument(std::uint32_t index) const;
    OptionalRef<const Context> getInstrumentContext(std::uint32_t index) const;
    OptionalRef<Context> getInstrumentContext(std::uint32_t index);
    std::optional<std::uint32_t> getInstrumentMainOutputChannelGroupIndex(std::uint32_t index) const;
    // Set a node as instrument of the channel. Only works if no instrument node
    // is set.
    bool setInstrument(std::uint32_t index,
    ade::NodeHandle nodeHandle, std::uint32_t outputChannelIndex);
    // Set a node as instrument of the channel and manage its context.
    // Only works if no instrument node is set.
    bool setInstrument(std::uint32_t index, ade::NodeHandle nodeHandle,
        Context&& context, std::uint32_t outputChannelIndex);
    // Detach an instrument node from its position and return its context (if
    // one is set). The context returned from this function is not managed by
    // `Mixer` anymore. Note that the detaching node is not removed.
    std::pair<ade::NodeHandle, Context> detachInstrument(std::uint32_t index);
    std::optional<IDGen::ID> instrumentAuxInputID(std::uint32_t channelIndex, std::uint32_t channelGroupIndex) const;
    std::optional<IDGen::ID> instrumentAuxOutputID(std::uint32_t channelIndex, std::uint32_t channelGroupIndex) const;
    bool isInstrumentBypassed(std::uint32_t index) const;
    void setInstrumentBypass(std::uint32_t index, bool bypass);
public:
    // `Mixer` will invoke this callback ONLY if batch disposer is not assigned
    // (i.e. `Mixer::setBatchUpdater` is not called).
    void setConnectionUpdatedCallback(ConnectionUpdatedCallback* callback);
    void resetConnectionUpdatedCallback();
    void connectionUpdated() const;
    void setPreInsertAudioInputChannelCallback(std::function<PreInsertChannelCallback>&& callback);
    void setPreInsertRegularChannelCallback(std::function<PreInsertChannelCallback>&& callback);
    void setPreInsertAudioOutputChannelCallback(std::function<PreInsertChannelCallback>&& callback);
    void resetPreInsertAudioInputChannelCallback();
    void resetPreInsertRegularChannelCallback();
    void resetPreInsertAudioOutputChannelCallback();
    void setSendAddedCallback(std::function<SendAddedCallback>&& callback);
    void setSendDestinationChangedCallback(std::function<SendDestinationChangedCallback>&& callback);
    void setSendRemovedCallback(std::function<SendRemovedCallback>&& callback);
    void resetSendAddedCallback();
    void resetSendDestinationChangedCallback();
    void resetSendRemovedCallback();
    void setAuxInputChangedCallback(std::function<AuxInputChangedCallback>&& callback);
    void setAuxOutputAddedCallback(std::function<AuxOutputAddedCallback>&& callback);
    void setAuxOutputDestinationChangedCallback(std::function<AuxOutputDestinationChangedCallback>&& callback);
    void setAuxOutputRemovedCallback(std::function<AuxOutputRemovedCallback>&& callback);
    void resetAuxInputChangedCallback();
    void resetAuxOutputAddedCallback();
    void resetAuxOutputDestinationChangedCallback();
    void resetAuxOutputRemovedCallback();
public:
    OptionalRef<YADAW::Util::BatchUpdater> batchUpdater();
    void setBatchUpdater(YADAW::Util::BatchUpdater& batchUpdater);
    void resetBatchUpdater();
public:
    std::optional<SendPosition> getSendPosition(IDGen::ID id) const;
public:
    std::optional<PluginAuxIOPosition> getAuxInputPosition(IDGen::ID id) const;
    std::optional<PluginAuxIOPosition> getAuxOutputPosition(IDGen::ID id) const;
    IDGen::ID getAuxInputPositionID(const PluginAuxIOPosition& position) const;
    IDGen::ID getAuxOutputPositionID(const PluginAuxIOPosition& position) const;
    std::optional<Position> getAuxInputSource(const PluginAuxIOPosition& position) const;
    bool setAuxInputSource(const PluginAuxIOPosition& position, Position source);
    OptionalRef<const std::vector<Position>> getAuxOutputDestinations(const PluginAuxIOPosition& position) const;
    bool addAuxOutputDestination(const PluginAuxIOPosition& position, Position destination);
    bool removeAuxOutputDestination(const PluginAuxIOPosition& position, std::uint32_t index, std::uint32_t removeCount = 1);
    bool setAuxOutputDestination(const PluginAuxIOPosition& position, std::uint32_t index, Position destination);
    void clearAuxOutputDestinations(const PluginAuxIOPosition& position);
private:
    Position& getAuxInputSource(const PluginAuxIOPosition& position);
    std::vector<Position>& getAuxOutputDestinations(const PluginAuxIOPosition& position);
    static void insertAdded(Inserts& sender, std::uint32_t position);
    static void insertAboutToBeRemoved(Inserts& sender, std::uint32_t position, std::uint32_t removeCount);
    static void insertRemoved(Inserts& sender, std::uint32_t position, std::uint32_t removeCount);
    void updatePluginAuxPosition(ChannelListType type, std::uint32_t fromChannelIndex);
    ade::NodeHandle getNodeFromPluginAuxPosition(const PluginAuxIOPosition& position) const;
private:
    using PolarityInverterAndNode = std::pair<
        std::unique_ptr<YADAW::Audio::Mixer::PolarityInverter>,
        ade::NodeHandle
    >;
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
private:
    template<typename T>
    SummingAndNode appendInputGroup(
        std::pair<std::unique_ptr<T>, ade::NodeHandle>& oldSummingAndNode,
        std::uint32_t appendCount = 1)
    {
        auto& [oldSumming, oldSummingNode] = oldSummingAndNode;
        auto& channelGroup = oldSumming->audioOutputGroupAt(0)->get();
        auto channelGroupType = channelGroup.type();
        auto channelCountInGroup = channelGroup.channelCount();
        auto newInputGroupCount = oldSumming->audioInputGroupCount() + appendCount;
        auto newSumming = std::make_unique<YADAW::Audio::Util::Summing>(
            newInputGroupCount, channelGroupType, channelCountInGroup
        );
        auto newSummingNode = graphWithPDC_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*newSumming));
        if(newInputGroupCount > 1)
        {
            auto multiInput = static_cast<YADAW::Audio::Engine::MultiInputDeviceWithPDC*>(
                graph_.getNodeData(newSummingNode).process.device()
            );
            multiInput->setBufferSize(bufferExtension().bufferSize());
        }
        auto inEdges = oldSummingNode->inEdges();
        std::uint32_t i = 0;
        for(auto inEdge: inEdges)
        {
            graph_.connect(inEdge->srcNode(), newSummingNode, graph_.getEdgeData(inEdge).fromChannel, i++);
        }
        return {std::move(newSumming), std::move(newSummingNode)};
    }
    template<typename T>
    std::optional<SummingAndNode> removeInputGroup(
        std::pair<std::unique_ptr<T>, ade::NodeHandle>& oldSummingAndNode,
        std::uint32_t channelGroupIndex, std::uint32_t removeCount = 1)
    {
        YADAW::Audio::Device::IAudioDevice* oldSumming = oldSummingAndNode.first.get();
        ade::NodeHandle oldSummingNode = oldSummingAndNode.second;
        auto oldInputCount = oldSumming->audioInputGroupCount();
        if(removeCount > 0 && channelGroupIndex + removeCount <= oldInputCount)
        {
            auto& channelGroup = oldSumming->audioOutputGroupAt(0)->get();
            auto channelGroupType = channelGroup.type();
            auto channelCountInGroup = channelGroup.channelCount();
            auto newInputGroupCount = oldSumming->audioInputGroupCount() - removeCount;
            auto newSumming = std::make_unique<YADAW::Audio::Util::Summing>(
                newInputGroupCount, channelGroupType, channelCountInGroup
            );
            auto newSummingNode = graphWithPDC_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*newSumming));
            if(newInputGroupCount > 1)
            {
                auto multiInput = static_cast<YADAW::Audio::Engine::MultiInputDeviceWithPDC*>(
                    graph_.getNodeData(newSummingNode).process.device()
                );
                multiInput->setBufferSize(bufferExtension().bufferSize());
            }
            auto inEdges = oldSummingNode->inEdges();
            for(const auto& inEdge: inEdges)
            {
                const auto& edgeData = graph_.getEdgeData(inEdge);
                if(auto oldToChannel = edgeData.toChannel;
                    oldToChannel < channelGroupIndex)
                {
                    graph_.connect(inEdge->srcNode(), newSummingNode, edgeData.fromChannel, oldToChannel);
                }
                else if(oldToChannel >= channelGroupIndex + removeCount)
                {
                    graph_.connect(inEdge->srcNode(), newSummingNode, edgeData.fromChannel, oldToChannel - removeCount);
                }
            }
            return {{std::move(newSumming), std::move(newSummingNode)}};
        }
        return std::nullopt;
    }
    std::tuple<PolarityInverterAndNode, MuteAndNode, FaderAndNode> createSend(
        ade::NodeHandle fromNode, ade::NodeHandle toNode,
        std::uint32_t fromChannel, std::uint32_t toChannel
    );
    template<typename T>
    SummingAndNode shrinkInputGroups(
        std::pair<std::unique_ptr<T>, ade::NodeHandle>& oldSummingAndNode)
    {
        YADAW::Audio::Device::IAudioDevice* oldSumming = oldSummingAndNode.first.get();
        ade::NodeHandle oldSummingNode = oldSummingAndNode.second;
        auto inEdges = oldSummingNode->inEdges();
        auto inputChannelCount = inEdges.size();
        auto& channelGroup = oldSumming->audioOutputGroupAt(0)->get();
        auto channelGroupType = channelGroup.type();
        auto channelCountInGroup = channelGroup.channelCount();
        auto newSumming = std::make_unique<YADAW::Audio::Util::Summing>(
            inputChannelCount, channelGroupType, channelCountInGroup
        );
        auto newSummingNode = graphWithPDC_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*newSumming));
        std::uint32_t toChannel = 0;
        for(const auto& inEdge: oldSummingNode->inEdges())
        {
            graph_.connect(
                inEdge->srcNode(), newSummingNode,
                graph_.getEdgeData(inEdge).fromChannel, toChannel++
            );
        }
        return {std::move(newSumming), std::move(newSummingNode)};
    }
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
        friend bool operator<(const IDAndIndex& lhs, IDGen::ID rhs)
        {
            return lhs.id < rhs;
        }
    };
private:
    YADAW::Audio::Engine::AudioDeviceGraph<
        YADAW::Audio::Engine::Extension::Buffer,
        YADAW::Audio::Engine::Extension::NameTag,
        YADAW::Audio::Engine::Extension::UpstreamLatency> graph_;
    YADAW::Audio::Engine::AudioDeviceGraphWithPDC graphWithPDC_;
    using SendPositions = std::map<IDGen::ID, SendPosition>;
    using SendPosIt = SendPositions::iterator;
    SendPositions sendPositions_;
    template<typename T>
    using SendContainer = std::array<std::vector<std::vector<T>>, 3>;
    using SendCollection = SendContainer<SendPosIt>;

    IDGen                             channelIDGen_                [3];
    Vec<IDGen::ID>                    channelIDs_                  [3];
    Vec<IDAndIndex>                   channelIDAndIndex_           [3];
    Vec<PolarityInverterAndNode>      channelPolarityInverters_    [3];
    Vec<std::unique_ptr<Inserts>>     channelPreFaderInserts_      [3];
    Vec<MuteAndNode>                  channelMutes_                [3];
    Vec<FaderAndNode>                 channelFaders_               [3];
    Vec<std::unique_ptr<Inserts>>     channelPostFaderInserts_     [3];
    Vec<MeterAndNode>                 channelMeters_               [3];
    Vec<ChannelInfo>                  channelInfos_                [3];
    Vec<Vec<SendPosIt>>               channelSendIDs_              [3];
    Vec<Vec<MuteAndNode>>             channelSendMutes_            [3];
    Vec<Vec<FaderAndNode>>            channelSendFaders_           [3];
    Vec<Vec<PolarityInverterAndNode>> channelSendPolarityInverters_[3];
    Vec<Vec<Position>>                channelSendDestinations_     [3];

    IDGen                                              &audioInputChannelIdGen_           = channelIDGen_                [AudioHardwareInputList];
    Vec<IDGen::ID>                                     &audioInputChannelId_              = channelIDs_                  [AudioHardwareInputList];
    Vec<IDAndIndex>                                    &audioInputChannelIdAndIndex_      = channelIDAndIndex_           [AudioHardwareInputList];
    Vec<PolarityInverterAndNode>                       &audioInputPolarityInverters_      = channelPolarityInverters_    [AudioHardwareInputList];
    Vec<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> &audioInputPreFaderInserts_        = channelPreFaderInserts_      [AudioHardwareInputList];
    Vec<MuteAndNode>                                   &audioInputMutes_                  = channelMutes_                [AudioHardwareInputList];
    Vec<FaderAndNode>                                  &audioInputFaders_                 = channelFaders_               [AudioHardwareInputList];
    Vec<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> &audioInputPostFaderInserts_       = channelPostFaderInserts_     [AudioHardwareInputList];
    Vec<MeterAndNode>                                  &audioInputMeters_                 = channelMeters_               [AudioHardwareInputList];
    Vec<ChannelInfo>                                   &audioInputChannelInfo_            = channelInfos_                [AudioHardwareInputList];
    Vec<Vec<SendPosIt>>                                &audioInputSendIDs_                = channelSendIDs_              [AudioHardwareInputList];
    Vec<Vec<MuteAndNode>>                              &audioInputSendMutes_              = channelSendMutes_            [AudioHardwareInputList];
    Vec<Vec<FaderAndNode>>                             &audioInputSendFaders_             = channelSendFaders_           [AudioHardwareInputList];
    Vec<Vec<PolarityInverterAndNode>>                  &audioInputSendPolarityInverters_  = channelSendPolarityInverters_[AudioHardwareInputList];
    Vec<Vec<Position>>                                 &audioInputSendDestinations_       = channelSendDestinations_     [AudioHardwareInputList];

    IDGen                                              &channelIdGen_                     = channelIDGen_                [RegularList];
    Vec<IDGen::ID>                                     &channelId_                        = channelIDs_                  [RegularList];
    Vec<IDAndIndex>                                    &channelIdAndIndex_                = channelIDAndIndex_           [RegularList];
    Vec<PolarityInverterAndNode>                       &polarityInverters_                = channelPolarityInverters_    [RegularList];
    Vec<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> &preFaderInserts_                  = channelPreFaderInserts_      [RegularList];
    Vec<MuteAndNode>                                   &mutes_                            = channelMutes_                [RegularList];
    Vec<FaderAndNode>                                  &faders_                           = channelFaders_               [RegularList];
    Vec<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> &postFaderInserts_                 = channelPostFaderInserts_     [RegularList];
    Vec<MeterAndNode>                                  &meters_                           = channelMeters_               [RegularList];
    Vec<ChannelInfo>                                   &channelInfo_                      = channelInfos_                [RegularList];
    Vec<Vec<SendPosIt>>                                &sendIDs_                          = channelSendIDs_              [RegularList];
    Vec<Vec<MuteAndNode>>                              &sendMutes_                        = channelSendMutes_            [RegularList];
    Vec<Vec<FaderAndNode>>                             &sendFaders_                       = channelSendFaders_           [RegularList];
    Vec<Vec<PolarityInverterAndNode>>                  &sendPolarityInverters_            = channelSendPolarityInverters_[RegularList];
    Vec<Vec<Position>>                                 &sendDestinations_                 = channelSendDestinations_     [RegularList];

    IDGen                                              &audioOutputChannelIdGen_          = channelIDGen_                [AudioHardwareOutputList];
    Vec<IDGen::ID>                                     &audioOutputChannelId_             = channelIDs_                  [AudioHardwareOutputList];
    Vec<IDAndIndex>                                    &audioOutputChannelIdAndIndex_     = channelIDAndIndex_           [AudioHardwareOutputList];
    Vec<PolarityInverterAndNode>                       &audioOutputPolarityInverters_     = channelPolarityInverters_    [AudioHardwareOutputList];
    Vec<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> &audioOutputPreFaderInserts_       = channelPreFaderInserts_      [AudioHardwareOutputList];
    Vec<MuteAndNode>                                   &audioOutputMutes_                 = channelMutes_                [AudioHardwareOutputList];
    Vec<FaderAndNode>                                  &audioOutputFaders_                = channelFaders_               [AudioHardwareOutputList];
    Vec<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> &audioOutputPostFaderInserts_      = channelPostFaderInserts_     [AudioHardwareOutputList];
    Vec<MeterAndNode>                                  &audioOutputMeters_                = channelMeters_               [AudioHardwareOutputList];
    Vec<ChannelInfo>                                   &audioOutputChannelInfo_           = channelInfos_                [AudioHardwareOutputList];
    Vec<Vec<SendPosIt>>                                &audioOutputSendIDs_               = channelSendIDs_              [AudioHardwareOutputList];
    Vec<Vec<MuteAndNode>>                              &audioOutputSendMutes_             = channelSendMutes_            [AudioHardwareOutputList];
    Vec<Vec<FaderAndNode>>                             &audioOutputSendFaders_            = channelSendFaders_           [AudioHardwareOutputList];
    Vec<Vec<PolarityInverterAndNode>>                  &audioOutputSendPolarityInverters_ = channelSendPolarityInverters_[AudioHardwareOutputList];
    Vec<Vec<Position>>                                 &audioOutputSendDestinations_      = channelSendDestinations_     [AudioHardwareOutputList];
    // Regular channels
    Vec<DeviceAndNode> inputDevices_;
    Vec<std::uint32_t> instrumentOutputChannelIndex_;
    Vec<Context>       instrumentContexts_;
    Vec<bool>          instrumentBypassed_;
    Vec<Position>      mainInput_;
    Vec<Position>      mainOutput_;
    // Audio hardware output channels
    Vec<SummingAndNode> audioOutputSummings_;

    IDGen auxInputIdGen_;
    IDGen auxOutputIdGen_;

    ConnectionUpdatedCallback* connectionUpdatedCallback_;
    std::function<PreInsertChannelCallback> preInsertChannelCallback_[3];
    std::function<PreInsertChannelCallback>& preInsertAudioInputChannelCallback_  = preInsertChannelCallback_[AudioHardwareInputList];
    std::function<PreInsertChannelCallback>& preInsertRegularChannelCallback_     = preInsertChannelCallback_[RegularList];
    std::function<PreInsertChannelCallback>& preInsertAudioOutputChannelCallback_ = preInsertChannelCallback_[AudioHardwareOutputList];
    std::function<SendAddedCallback> sendAddedCallback_;
    std::function<SendDestinationChangedCallback> sendDestinationChangedCallback_;
    std::function<SendRemovedCallback> sendRemovedCallback_;
    std::function<AuxInputChangedCallback> auxInputChangedCallback_;
    std::function<AuxOutputAddedCallback> auxOutputAddedCallback_;
    std::function<AuxOutputDestinationChangedCallback> auxOutputDestinationChangedCallback_;
    std::function<AuxOutputRemovedCallback> auxOutputRemovedCallback_;
    std::function<DeviceFactoryType<VolumeFader>> volumeFaderFactory_;
    std::function<DeviceFactoryType<Meter>> meterFactory_;

    using PluginAuxPosIDs = std::map<IDGen::ID, PluginAuxIOPosition>;
    PluginAuxPosIDs pluginAuxInputIDs_;
    PluginAuxPosIDs pluginAuxOutputIDs_;

    using PluginAuxPosIt = PluginAuxPosIDs::iterator;
    template<typename T>
    using PluginAuxIOContainer = std::array< // channel type
        std::vector<std::pair<               // channel index
            Vec<T>,                          // instruments -> channel group index
            Vec<                             // inserts -> inserts index
                Vec<                         // insert index
                    Vec<T>                   // channel group index
                >
            >
        >>, 3
    >;
    using PluginAuxCollection = PluginAuxIOContainer<PluginAuxPosIt>;
    PluginAuxCollection pluginAuxInputs_;
    PluginAuxCollection pluginAuxOutputs_;
    using PluginAuxInputSources = PluginAuxIOContainer<Position>;
    using PluginAuxOutputDestinations = PluginAuxIOContainer<std::vector<Position>>;
    PluginAuxInputSources pluginAuxInputSources_;
    PluginAuxOutputDestinations pluginAuxOutputDestinations_;
    YADAW::Util::BatchUpdater* batchUpdater_ = nullptr;
    IDGen sendIDGen_;
};
}

#endif // YADAW_SRC_AUDIO_MIXER_MIXER
