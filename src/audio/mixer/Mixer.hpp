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
#include "util/OptionalUtil.hpp"

#include <QColor>

#include <cstdint>
#include <concepts>
#include <optional>

namespace YADAW::Audio::Mixer
{
class Mixer;

namespace Impl
{
void blankNodeAddedCallback(const Mixer&);
void blankNodeRemovedCallback(const Mixer&);
void blankConnectionUpdatedCallback(const Mixer&);
}

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
//            2. A not-owned mute/solo of the The external audio in.
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

// TODO: Improve the `Mixer` API so that complex operations can be done more
//       efficiently.
//       For now, these operations work as follows:
//       1. `Mixer` removes the nodes of the unused devices, but doesn't dispose
//          those devices.
//       2. Call the callback provided outside `Mixer` to make sure those
//          disposing devices are not used anymore.
//       3. Dispose unused devices.
//       For now, the callback provided outside runs on the UI thread, and it
//       works as follows:
//       1. Export the device graph in `Mixer` to process sequences used by
//          the audio thread.
//       2. Pass the process sequence to the audio thread, wait the audio thread
//          to grab the new process sequence, which means the old process
//          sequence (with unused devices) is not used. The audio thread grabs
//          the new process sequence in the next audio callback, which means
//          that the UI thread callback might block for (at most) the duration
//          of the timespan of two audio adjacent callbacks, which is often
//          measured in milliseconds.
//       At first, `Mixer` can add only one channel at a time. If the user wants
//       to add multiple channels, then `Mixer::insertChannel` is called
//       repeatedly (for the number of new channels). The user has to wait for
//       multiple audio callbacks before new channels are added and the UI
//       thread does not freeze anymore.
//       To solve this problem, I added `Mixer::insertChannels` that adds
//       multiple channels with only one call to the callback.
//       I had already provided `Mixer::removeChannel` which can remove multiple
//       continuous channels at a time with only one call to the callback. But
//       if the user wants to remove multiple channels that are not continuous,
//       then `Mixer` still has to call the callback for multiple times. I can
//       solve this by adding even more member functions, but the user operation
//       can get more and more complex, which in return makes the mixer API more
//       and more complex as well, without solving the root problem. I don't
//       think it's the ultimate solution.
//       I want to solve the problem listed above without making the `Mixer` API
//       too complicated. Looking for a better way.
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
    struct Position
    {
        enum Type: std::uint32_t
        {
            Invalid,
            AudioHardwareIOChannel,
            BusAndFXChannel,
            PluginAuxIO
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
    // using VolumeFaderFactoryCallback = std::unique_ptr<VolumeFader>(
    //     YADAW::Audio::Base::ChannelGroupType channelGroupType,
    //     std::uint32_t channelCountInGroup
    // );
    // using MeterFactoryCallback = std::unique_ptr<Meter>(
    //     YADAW::Audio::Base::ChannelGroupType channelGroupType,
    //     std::uint32_t channelCountInGroup
    // );
public:
    template<
        DeviceFactory<VolumeFader> VolumeFaderFactory,
        DeviceFactory<Meter> MeterFactory
    >
    Mixer(
        VolumeFaderFactory&& volumeFaderFactory,
        MeterFactory&& meterFactory):
        graph_(),
        graphWithPDC_(graph_),
        connectionUpdatedCallback_(&Impl::blankConnectionUpdatedCallback),
        volumeFaderFactory_(std::move(volumeFaderFactory)),
        meterFactory_(std::move(meterFactory))
    {
    }
public:
    const YADAW::Audio::Engine::AudioDeviceGraphWithPDC& graph() const;
    YADAW::Audio::Engine::AudioDeviceGraphWithPDC& graph();
    const YADAW::Audio::Engine::Extension::Buffer& bufferExtension() const;
    YADAW::Audio::Engine::Extension::Buffer& bufferExtension();
    const YADAW::Audio::Engine::Extension::NameTag& nameTagExtension() const;
    YADAW::Audio::Engine::Extension::NameTag& nameTag();
public:
    std::uint32_t audioInputChannelCount() const;
    std::uint32_t channelCount() const;
    std::uint32_t audioOutputChannelCount() const;
    OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> audioInputChannelPolarityInverterAt(std::uint32_t index) const;
    OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> audioOutputChannelPolarityInverterAt(std::uint32_t index) const;
    OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> channelPolarityInverterAt(std::uint32_t index) const;
    OptionalRef<YADAW::Audio::Mixer::PolarityInverter> audioInputChannelPolarityInverterAt(std::uint32_t index);
    OptionalRef<YADAW::Audio::Mixer::PolarityInverter> audioOutputChannelPolarityInverterAt(std::uint32_t index);
    OptionalRef<YADAW::Audio::Mixer::PolarityInverter> channelPolarityInverterAt(std::uint32_t index);
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
    std::optional<const std::uint32_t> audioInputChannelSendCount(std::uint32_t index) const;
    std::optional<const std::uint32_t> channelSendCount(std::uint32_t index) const;
    std::optional<const std::uint32_t> audioOutputChannelSendCount(std::uint32_t index) const;
    std::optional<const bool> audioInputChannelSendIsPreFader(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    std::optional<const bool> channelSendIsPreFader(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    std::optional<const bool> audioOutputChannelSendIsPreFader(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    std::optional<const Position> audioInputChannelSendDestination(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    std::optional<const Position> channelSendDestination(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    std::optional<const Position> audioOutputChannelSendDestination(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<const VolumeFader> audioInputChannelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<const VolumeFader> ChannelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<const VolumeFader> audioOutputChannelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<VolumeFader> audioInputChannelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<VolumeFader> channelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<VolumeFader> audioOutputChannelSendFaderAt(std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<const YADAW::Audio::Util::Mute> audioInputChannelSendMuteAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<const YADAW::Audio::Util::Mute> channelSendMuteAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<const YADAW::Audio::Util::Mute> audioOutputChannelSendMuteAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<YADAW::Audio::Util::Mute> audioInputChannelSendMuteAt(std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<YADAW::Audio::Util::Mute> channelSendMuteAt(std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<YADAW::Audio::Util::Mute> audioOutputChannelSendMuteAt(std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<const PolarityInverter> audioInputChannelSendPolarityInverterAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<const PolarityInverter> channelSendPolarityInverterAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<const PolarityInverter> audioOutputChannelSendPolarityInverterAt(std::uint32_t channelIndex, std::uint32_t sendIndex) const;
    OptionalRef<PolarityInverter> audioInputChannelSendPolarityInverterAt(std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<PolarityInverter> channelSendPolarityInverterAt(std::uint32_t channelIndex, std::uint32_t sendIndex);
    OptionalRef<PolarityInverter> audioOutputChannelSendPolarityInverterAt(std::uint32_t channelIndex, std::uint32_t sendIndex);
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
    OptionalRef<const Position> mainInputAt(std::uint32_t index) const;
    bool setMainInputAt(std::uint32_t index, Position position);
    OptionalRef<const Position> mainOutputAt(std::uint32_t index) const;
    bool setMainOutputAt(std::uint32_t index, Position position);
    std::optional<std::uint32_t> getInputIndexOfId(IDGen::ID id) const;
    std::optional<std::uint32_t> getOutputIndexOfId(IDGen::ID id) const;
    std::optional<std::uint32_t> getIndexOfId(IDGen::ID id) const;
    bool hasMuteInAudioInputChannels() const;
    bool hasMuteInRegularChannels() const;
    bool hasMuteInAudioOutputChannels() const;
    void unmuteAudioInputChannels();
    void unmuteRegularChannels();
    void unmuteAudioOutputChannels();
    void unmuteAllChannels();
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
    bool removeChannel(
        std::uint32_t position,
        std::uint32_t removeCount = 1
    );
    void clearChannels();
public:
    std::optional<bool> appendAudioInputChannelSend(std::uint32_t channelIndex, bool isPreFader, Position destination);
    std::optional<bool> insertAudioInputChannelSend(std::uint32_t channelIndex, std::uint32_t sendPosition, bool isPreFader, Position destination);
    std::optional<bool> setAudioInputChannelSendPreFader(std::uint32_t channelIndex, std::uint32_t sendIndex, bool preFader);
    std::optional<bool> setAudioInputChannelSendDestination(std::uint32_t channelIndex, std::uint32_t sendIndex, Position destination);
    std::optional<bool> removeAudioInputChannelSend(std::uint32_t channelIndex, std::uint32_t sendPosition, std::uint32_t removeCount = 1);
    std::optional<bool> clearAudioInputChannelSends(std::uint32_t channelIndex);
    std::optional<bool> appendChannelSend(std::uint32_t channelIndex, bool isPreFader, Position destination);
    std::optional<bool> insertChannelSend(std::uint32_t channelIndex, std::uint32_t sendPosition, bool isPreFader, Position destination);
    std::optional<bool> setChannelSendPreFader(std::uint32_t channelIndex, std::uint32_t sendIndex, bool preFader);
    std::optional<bool> setChannelSendDestination(std::uint32_t channelIndex, std::uint32_t sendIndex, Position destination);
    std::optional<bool> removeChannelSend(std::uint32_t channelIndex, std::uint32_t sendPosition, std::uint32_t removeCount = 1);
    std::optional<bool> clearChannelSends(std::uint32_t channelIndex);
    std::optional<bool> appendAudioOutputChannelSend(std::uint32_t channelIndex, bool isPreFader, Position destination);
    std::optional<bool> insertAudioOutputChannelSend(std::uint32_t channelIndex, std::uint32_t sendPosition, bool isPreFader, Position destination);
    std::optional<bool> setAudioOutputChannelSendPreFader(std::uint32_t channelIndex, std::uint32_t sendIndex, bool preFader);
    std::optional<bool> setAudioOutputChannelSendDestination(std::uint32_t channelIndex, std::uint32_t sendIndex, Position destination);
    std::optional<bool> removeAudioOutputChannelSend(std::uint32_t channelIndex, std::uint32_t sendPosition, std::uint32_t removeCount = 1);
    std::optional<bool> clearAudioOutputChannelSends(std::uint32_t channelIndex);
public:
    ade::NodeHandle getInstrument(std::uint32_t index) const;
    bool setInstrument(std::uint32_t index,
        ade::NodeHandle nodeHandle, std::uint32_t outputChannelIndex);
    ade::NodeHandle removeInstrument(std::uint32_t index);
    bool isInstrumentBypassed(std::uint32_t index) const;
    void setInstrumentBypass(std::uint32_t index, bool bypass);
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

    IDGen audioInputChannelIdGen_;
    std::vector<IDGen::ID> audioInputChannelId_;
    std::vector<IDAndIndex> audioInputChannelIdAndIndex_;
    std::vector<PolarityInverterAndNode> audioInputPolarityInverters_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioInputPreFaderInserts_;
    std::vector<MuteAndNode> audioInputMutes_;
    std::vector<FaderAndNode> audioInputFaders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioInputPostFaderInserts_;
    std::vector<MeterAndNode> audioInputMeters_;
    std::vector<ChannelInfo> audioInputChannelInfo_;
    std::vector<std::vector<MuteAndNode>> audioInputSendMutes_;
    std::vector<std::vector<FaderAndNode>> audioInputSendFaders_;
    std::vector<std::vector<PolarityInverterAndNode>> audioInputSendPolarityInverters_;
    std::vector<std::vector<Position>> audioInputSendDestinations_;

    IDGen channelIdGen_;
    std::vector<IDGen::ID> channelId_;
    std::vector<IDAndIndex> channelIdAndIndex_;
    std::vector<DeviceAndNode> inputDevices_;
    std::vector<std::uint32_t> instrumentOutputChannelIndex_;
    std::vector<bool> instrumentBypassed_;
    std::vector<PolarityInverterAndNode> polarityInverters_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> preFaderInserts_;
    std::vector<MuteAndNode> mutes_;
    std::vector<FaderAndNode> faders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> postFaderInserts_;
    std::vector<MeterAndNode> meters_;
    std::vector<ChannelInfo> channelInfo_;
    std::vector<Position> mainInput_;
    std::vector<Position> mainOutput_;
    std::vector<std::vector<MuteAndNode>> sendMutes_;
    std::vector<std::vector<FaderAndNode>> sendFaders_;
    std::vector<std::vector<PolarityInverterAndNode>> sendPolarityInverters_;
    std::vector<std::vector<Position>> sendDestinations_;

    IDGen audioOutputChannelIdGen_;
    std::vector<IDGen::ID> audioOutputChannelId_;
    std::vector<IDAndIndex> audioOutputChannelIdAndIndex_;
    std::vector<SummingAndNode> audioOutputSummings_;
    std::vector<PolarityInverterAndNode> audioOutputPolarityInverters_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioOutputPreFaderInserts_;
    std::vector<MuteAndNode> audioOutputMutes_;
    std::vector<FaderAndNode> audioOutputFaders_;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> audioOutputPostFaderInserts_;
    std::vector<MeterAndNode> audioOutputMeters_;
    std::vector<ChannelInfo> audioOutputChannelInfo_;
    std::vector<std::vector<MuteAndNode>> audioOutputSendMutes_;
    std::vector<std::vector<FaderAndNode>> audioOutputSendFaders_;
    std::vector<std::vector<PolarityInverterAndNode>> audioOutputSendPolarityInverters_;
    std::vector<std::vector<Position>> audioOutputSendDestinations_;

    std::unordered_set<ade::EdgeHandle, ade::HandleHasher<ade::Edge>> connections_;

    ConnectionUpdatedCallback* connectionUpdatedCallback_;
    std::function<DeviceFactoryType<VolumeFader>> volumeFaderFactory_;
    std::function<DeviceFactoryType<Meter>> meterFactory_;
};
}

#endif //YADAW_SRC_AUDIO_MIXER_MIXER
