#include "Mixer.hpp"

#include "audio/mixer/BlankGenerator.hpp"
#include "audio/mixer/BlankReceiver.hpp"

namespace YADAW::Audio::Mixer
{
void blankNodeAddedCallback(const Mixer&) {}
void blankNodeRemovedCallback(const Mixer&) {}
void blankConnectionUpdatedCallback(const Mixer&) {}

Mixer::Mixer():
    graph_(),
    graphWithPDC_(graph_),
    nodeAddedCallback_(&blankNodeAddedCallback),
    nodeRemovedCallback_(&blankNodeRemovedCallback),
    connectionUpdatedCallback_(&blankConnectionUpdatedCallback)
{}

const YADAW::Audio::Engine::AudioDeviceGraphWithPDC& Mixer::graph() const
{
    return graphWithPDC_;
}

YADAW::Audio::Engine::AudioDeviceGraphWithPDC& Mixer::graph()
{
    return graphWithPDC_;
}

const YADAW::Audio::Engine::Extension::Buffer& Mixer::bufferExtension() const
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
}

YADAW::Audio::Engine::Extension::Buffer& Mixer::bufferExtension()
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::Buffer>();
}

std::uint32_t Mixer::audioInputChannelCount() const
{
    return audioInputFaders_.size();
}

std::uint32_t Mixer::channelCount() const
{
    return faders_.size();
}

std::uint32_t Mixer::audioOutputChannelCount() const
{
    return audioOutputFaders_.size();
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPreFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPostFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::channelPreFaderInsertsAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*preFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::channelPostFaderInsertsAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*postFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPreFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPostFaderInsertsAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPreFaderInsertsAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioInputChannelPostFaderInsertsAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::channelPreFaderInsertsAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*preFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::channelPostFaderInsertsAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*postFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPreFaderInsertsAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPreFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts>
    Mixer::audioOutputChannelPostFaderInsertsAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPostFaderInserts_[index])};
    }
    return std::nullopt;
}

OptionalRef<const Mixer::ChannelInfo> Mixer::audioInputChannelInfoAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(audioInputChannelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<const Mixer::ChannelInfo> Mixer::audioOutputChannelInfoAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(audioOutputChannelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<const Mixer::ChannelInfo> Mixer::channelInfoAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(channelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<Mixer::ChannelInfo> Mixer::audioInputChannelInfoAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(audioInputChannelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<Mixer::ChannelInfo> Mixer::audioOutputChannelInfoAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(audioOutputChannelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<Mixer::ChannelInfo> Mixer::channelInfoAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(channelInfo_[index])};
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::audioInputVolumeFaderAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputFaders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::audioOutputVolumeFaderAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputFaders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::volumeFaderAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*(faders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::audioInputVolumeFaderAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputFaders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::audioOutputVolumeFaderAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputFaders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::volumeFaderAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*(faders_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::audioInputMuteAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputMutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::audioOutputMuteAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputMutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::muteAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*(mutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::audioInputMuteAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputMutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::audioOutputMuteAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputMutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::muteAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*(mutes_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const Meter> Mixer::audioInputMeterAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*(audioInputMeters_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const Meter> Mixer::audioOutputMeterAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*(audioOutputMeters_[index].first))};
    }
    return std::nullopt;
}

OptionalRef<const Meter> Mixer::meterAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*(meters_[index].first))};
    }
    return std::nullopt;
}

std::optional<YADAW::Audio::Base::ChannelGroupType>
Mixer::audioInputChannelGroupType(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {audioInputMutes_[index].first->audioInputGroupAt(0)->get().type()};
    }
    return std::nullopt;
}

std::optional<YADAW::Audio::Base::ChannelGroupType> Mixer::audioOutputChannelGroupType(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {audioOutputMutes_[index].first->audioInputGroupAt(0)->get().type()};
    }
    return std::nullopt;
}

std::optional<YADAW::Audio::Base::ChannelGroupType> Mixer::channelGroupType(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {mutes_[index].first->audioInputGroupAt(0)->get().type()};
    }
    return std::nullopt;
}

std::optional<YADAW::Util::AutoIncrementID::ID> Mixer::audioInputChannelID(
    std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {audioInputChannelId_[index]};
    }
    return std::nullopt;
}

std::optional<YADAW::Util::AutoIncrementID::ID> Mixer::audioOutputChannelID(
    std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {audioOutputChannelId_[index]};
    }
    return std::nullopt;
}

std::optional<YADAW::Util::AutoIncrementID::ID> Mixer::channelID(
    std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {channelId_[index]};
    }
    return std::nullopt;
}

bool Mixer::appendAudioInputChannel(
    const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex)
{
    return insertAudioInputChannel(audioInputChannelCount(), inNode, channelGroupIndex);
}

bool Mixer::insertAudioInputChannel(std::uint32_t position,
    const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex)
{
    auto device = graph_.getNodeData(inNode).process.device();
    if(channelGroupIndex < device->audioOutputGroupCount()
       && position <= audioInputChannelCount())
    {
        auto it = audioInputChannelId_.emplace(
            audioInputChannelId_.begin() + position,
            audioInputChannelIdGen_()
        );
        audioInputChannelIdAndIndex_.emplace(
            std::lower_bound(
                audioInputChannelIdAndIndex_.begin(),
                audioInputChannelIdAndIndex_.end(),
                *it,
                [](IDAndIndex idAndIndex, IDGen::ID id)
                {
                    return idAndIndex.id < id;
                }
            ),
            *it, position
        );
        const auto& channelGroup = device->audioOutputGroupAt(channelGroupIndex)->get();
        auto meter = std::make_unique<YADAW::Audio::Mixer::Meter>(
            8192, channelGroup.type(), channelGroup.channelCount()
        );
        auto meterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*meter)
        );
        auto fader = std::make_unique<YADAW::Audio::Mixer::VolumeFader>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto faderNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*fader)
        );
        auto mute = std::make_unique<YADAW::Audio::Util::Mute>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto muteNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*mute)
        );
        audioInputPreFaderInserts_.emplace(
            audioInputPreFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, inNode, muteNode, channelGroupIndex, 0
            )
        );
        audioInputMutes_.emplace(
            audioInputMutes_.begin() + position,
            std::move(mute), muteNode
        );
        graph_.connect(muteNode, faderNode, 0, 0);
        audioInputPostFaderInserts_.emplace(
            audioInputPostFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            )
        );
        audioInputFaders_.emplace(
            audioInputFaders_.begin() + position,
            std::move(fader), faderNode
        );
        audioInputMeters_.emplace(
            audioInputMeters_.begin() + position,
            std::move(meter), meterNode
        );
        nodeAddedCallback_(*this);
        auto& info = *audioInputChannelInfo_.emplace(audioInputChannelInfo_.begin() + position);
        info.channelType = ChannelType::AudioBus;
        return true;
    }
    return false;
}

bool Mixer::removeAudioInputChannel(
    std::uint32_t first, std::uint32_t removeCount)
{
    if(auto last = first + removeCount;
        first < audioInputChannelCount()
            && last <= audioInputChannelCount()
    )
    {
        std::vector<ade::NodeHandle> nodesToRemove;
        nodesToRemove.reserve(removeCount * 4);
        FOR_RANGE(i, first, last)
        {
            nodesToRemove.emplace_back(audioInputPreFaderInserts_[i]->inNode());
            nodesToRemove.emplace_back(audioInputMutes_[i].second);
            nodesToRemove.emplace_back(audioInputFaders_[i].second);
            nodesToRemove.emplace_back(audioInputMeters_[i].second);
        }
        audioInputPreFaderInserts_.erase(
            audioInputPreFaderInserts_.begin() + first,
            audioInputPreFaderInserts_.begin() + last
        );
        audioInputPostFaderInserts_.erase(
            audioInputPostFaderInserts_.begin() + first,
            audioInputPostFaderInserts_.begin() + last
        );
        FOR_RANGE0(i, nodesToRemove.size())
        {
            graphWithPDC_.removeNode(nodesToRemove[i]);
        }
        nodeRemovedCallback_(*this);
        audioInputMutes_.erase(
            audioInputMutes_.begin() + first,
            audioInputMutes_.begin() + last
        );
        audioInputFaders_.erase(
            audioInputFaders_.begin() + first,
            audioInputFaders_.begin() + last
        );
        audioInputMeters_.erase(
            audioInputMeters_.begin() + first,
            audioInputMeters_.begin() + last
        );
        audioInputChannelInfo_.erase(
            audioInputChannelInfo_.begin() + first,
            audioInputChannelInfo_.begin() + last
        );
        std::sort(
            audioInputChannelId_.begin() + first,
            audioInputChannelId_.begin() + last
        );
        auto it2 = audioInputChannelId_.begin() + first;
        audioInputChannelIdAndIndex_.erase(
            std::remove_if(
                audioInputChannelIdAndIndex_.begin(),
                audioInputChannelIdAndIndex_.end(),
                [&it2](IDAndIndex idAndIndex)
                {
                    if(idAndIndex.id == *it2)
                    {
                        ++it2;
                        return true;
                    }
                    return false;
                }
            ),
            audioInputChannelIdAndIndex_.end()
        );
        audioInputChannelId_.erase(
            audioInputChannelId_.begin() + first,
            audioInputChannelId_.begin() + last
        );
        return true;
    }
    return false;
}

void Mixer::clearAudioInputChannels()
{
    removeAudioInputChannel(0, audioInputChannelCount());
}

bool Mixer::appendAudioOutputChannel(
    const ade::NodeHandle& outNode, std::uint32_t channelGroupIndex)
{
    return insertAudioOutputChannel(audioOutputChannelCount(), outNode, channelGroupIndex);
}

bool Mixer::insertAudioOutputChannel(std::uint32_t position,
    const ade::NodeHandle& outNode, std::uint32_t channel)
{
    // input device of an audio output channel is a summing.
    auto device = graph_.getNodeData(outNode).process.device();
    if(channel < device->audioInputGroupCount()
        && position <= audioOutputChannelCount())
    {
        auto it = audioOutputChannelId_.emplace(
            audioOutputChannelId_.begin() + position,
            audioOutputChannelIdGen_()
        );
        audioOutputChannelIdAndIndex_.emplace(
            std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                *it,
                [](IDAndIndex idAndIndex, IDGen::ID id)
                {
                    return idAndIndex.id < id;
                }
            ),
            *it, position
        );
        const auto& channelGroup = device->audioInputGroupAt(channel)->get();
        auto summing = std::make_unique<YADAW::Audio::Util::Summing>(
            0, channelGroup.type(), channelGroup.channelCount()
        );
        auto summingNode = graphWithPDC_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*summing)
        );
        auto meter = std::make_unique<YADAW::Audio::Mixer::Meter>(
            8192, channelGroup.type(), channelGroup.channelCount()
        );
        auto meterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*meter)
        );
        auto fader = std::make_unique<YADAW::Audio::Mixer::VolumeFader>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto faderNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*fader)
        );
        auto mute = std::make_unique<YADAW::Audio::Util::Mute>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto muteNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*mute)
        );
        audioOutputPreFaderInserts_.emplace(
            audioOutputPreFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, summingNode, muteNode, 0, 0
            )
        );
        audioOutputMutes_.emplace(
            audioOutputMutes_.begin() + position,
            std::move(mute), muteNode
        );
        graph_.connect(muteNode, faderNode, 0, 0);
        audioOutputPostFaderInserts_.emplace(
            audioOutputPostFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            )
        );
        graph_.connect(meterNode, outNode, 0, channel);
        audioOutputSummings_.emplace(
            audioOutputSummings_.begin() + position,
            std::move(summing), summingNode
        );
        audioOutputFaders_.emplace(
            audioOutputFaders_.begin() + position,
            std::move(fader), faderNode
        );
        audioOutputMeters_.emplace(
            audioOutputMeters_.begin() + position,
            std::move(meter), meterNode
        );
        nodeAddedCallback_(*this);
        auto& info = *audioOutputChannelInfo_.emplace(audioOutputChannelInfo_.begin() + position);
        info.channelType = ChannelType::AudioBus;
        return true;
    }
    return false;
}

bool Mixer::removeAudioOutputChannel(
    std::uint32_t first, std::uint32_t removeCount)
{
    if(auto last = first + removeCount;
        first < audioOutputChannelCount()
            && last <= audioOutputChannelCount()
    )
    {
        std::vector<ade::NodeHandle> nodesToRemove;
        nodesToRemove.reserve(removeCount * 5);
        FOR_RANGE(i, first, last)
        {
            nodesToRemove.emplace_back(audioOutputSummings_[i].second);
            nodesToRemove.emplace_back(audioOutputMutes_[i].second);
            nodesToRemove.emplace_back(audioOutputFaders_[i].second);
            nodesToRemove.emplace_back(audioOutputMeters_[i].second);
            nodesToRemove.emplace_back(
                *(audioOutputMeters_[i].second->outNodes().begin())
            );
        }
        audioOutputPreFaderInserts_.erase(
            audioOutputPreFaderInserts_.begin() + first,
            audioOutputPreFaderInserts_.begin() + last
        );
        audioOutputPostFaderInserts_.erase(
            audioOutputPostFaderInserts_.begin() + first,
            audioOutputPostFaderInserts_.begin() + last
        );
        FOR_RANGE0(i, nodesToRemove.size())
        {
            graphWithPDC_.removeNode(nodesToRemove[i]);
        }
        nodeRemovedCallback_(*this);
        audioOutputMutes_.erase(
            audioOutputMutes_.begin() + first,
            audioOutputMutes_.begin() + last
        );
        audioOutputSummings_.erase(
            audioOutputSummings_.begin() + first,
            audioOutputSummings_.begin() + last
        );
        audioOutputFaders_.erase(
            audioOutputFaders_.begin() + first,
            audioOutputFaders_.begin() + last
        );
        audioOutputMeters_.erase(
            audioOutputMeters_.begin() + first,
            audioOutputMeters_.begin() + last
        );
        audioOutputChannelInfo_.erase(
            audioOutputChannelInfo_.begin() + first,
            audioOutputChannelInfo_.begin() + last
        );
        std::sort(
            audioOutputChannelId_.begin() + first,
            audioOutputChannelId_.begin() + last
        );
        auto it2 = audioOutputChannelId_.begin() + first;
        audioOutputChannelIdAndIndex_.erase(
            std::remove_if(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                [&it2](IDAndIndex idAndIndex)
                {
                    if(idAndIndex.id == *it2)
                    {
                        ++it2;
                        return true;
                    }
                    return false;
                }
            ),
            audioOutputChannelIdAndIndex_.end()
        );
        audioOutputChannelId_.erase(
            audioOutputChannelId_.begin() + first,
            audioOutputChannelId_.begin() + last
        );
        return true;
    }
    return false;
}

void Mixer::clearAudioOutputChannels()
{
    removeAudioOutputChannel(0, audioOutputChannelCount());
}

bool Mixer::insertChannel(
    std::uint32_t position, Mixer::ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    auto ret = false;
    if(position <= channelCount())
    {
        switch(channelType)
        {
        case ChannelType::Audio:
        case ChannelType::Instrument:
        {
            auto blankGenerator = new (std::nothrow) YADAW::Audio::Mixer::BlankGenerator(
                channelGroupType, channelCountInGroup
            );
            if(!blankGenerator)
            {
                break;
            }
            std::unique_ptr<YADAW::Audio::Device::IAudioDevice> inputDevice(blankGenerator);
            auto blankReceiver = new (std::nothrow) YADAW::Audio::Mixer::BlankReceiver(
                channelGroupType, channelCountInGroup
            );
            if(!blankReceiver)
            {
                break;
            }
            std::unique_ptr<YADAW::Audio::Device::IAudioDevice> outputDevice(blankReceiver);
            auto inputDeviceNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*blankGenerator));
            auto outputDeviceNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*blankReceiver));
            auto fader = std::make_unique<YADAW::Audio::Mixer::VolumeFader>(
                channelGroupType, channelCountInGroup
            );
            auto faderNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*fader));
            auto meter = std::make_unique<YADAW::Audio::Mixer::Meter>(
                8192, channelGroupType, channelCountInGroup
            );
            auto meterNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*meter));
            auto mute = std::make_unique<YADAW::Audio::Util::Mute>(
                channelGroupType, channelCountInGroup
            );
            auto muteNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*mute));
            auto preFaderInserts = std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, inputDeviceNode, muteNode, 0, 0
            );
            graph_.connect(muteNode, faderNode, 0, 0);
            auto postFaderInserts = std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            );
            inputDevices_.emplace(inputDevices_.begin() + position,
                std::move(inputDevice), inputDeviceNode
            );
            preFaderInserts_.emplace(preFaderInserts_.begin() + position,
                std::move(preFaderInserts)
            );
            faders_.emplace(faders_.begin() + position,
                std::move(fader), faderNode
            );
            postFaderInserts_.emplace(postFaderInserts_.begin() + position,
                std::move(postFaderInserts)
            );
            meters_.emplace(meters_.begin() + position,
                std::move(meter), meterNode
            );
            mutes_.emplace(mutes_.begin() + position,
                std::move(mute), muteNode
            );
            outputDevices_.emplace(outputDevices_.begin() + position,
                std::move(outputDevice), outputDeviceNode
            );
            ret = true;
            break;
        }
        case ChannelType::AudioFX:
        case ChannelType::AudioBus:
        {
            auto summing = new (std::nothrow) YADAW::Audio::Util::Summing(
                1, channelGroupType, channelCountInGroup
            );
            if(!summing)
            {
                break;
            }
            std::unique_ptr<YADAW::Audio::Device::IAudioDevice> inputDevice(summing);
            auto blankReceiver = new (std::nothrow) YADAW::Audio::Mixer::BlankReceiver(
                channelGroupType, channelCountInGroup
            );
            if(!blankReceiver)
            {
                break;
            }
            std::unique_ptr<YADAW::Audio::Device::IAudioDevice> outputDevice(blankReceiver);
            auto inputDeviceNode = graphWithPDC_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*summing));
            auto outputDeviceNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*blankReceiver));
            auto fader = std::make_unique<YADAW::Audio::Mixer::VolumeFader>(
                channelGroupType, channelCountInGroup
            );
            auto faderNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*fader));
            auto meter = std::make_unique<YADAW::Audio::Mixer::Meter>(
                8192, channelGroupType, channelCountInGroup
            );
            auto meterNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*meter));
            auto mute = std::make_unique<YADAW::Audio::Util::Mute>(
                channelGroupType, channelCountInGroup
            );
            auto muteNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*mute));
            auto preFaderInserts = std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, inputDeviceNode, muteNode, 0, 0
            );
            graph_.connect(muteNode, faderNode, 0, 0);
            auto postFaderInserts = std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            );
            inputDevices_.emplace(inputDevices_.begin() + position,
                std::move(inputDevice), inputDeviceNode
            );
            preFaderInserts_.emplace(preFaderInserts_.begin() + position,
                std::move(preFaderInserts)
            );
            faders_.emplace(faders_.begin() + position,
                std::move(fader), faderNode
            );
            postFaderInserts_.emplace(postFaderInserts_.begin() + position,
                std::move(postFaderInserts)
            );
            meters_.emplace(meters_.begin() + position,
                std::move(meter), meterNode
            );
            mutes_.emplace(mutes_.begin() + position,
                std::move(mute), muteNode
            );
            outputDevices_.emplace(outputDevices_.begin() + position,
                std::move(outputDevice), outputDeviceNode
            );
            ret = true;
            break;
        }
        }
    }
    if(ret)
    {
        auto channelInfoIt = channelInfo_.emplace(channelInfo_.begin() + position);
        channelInfoIt->channelType = channelType;
        auto idIt = channelId_.emplace(
            channelId_.begin() + position,
            channelIdGen_()
        );
        channelIdAndIndex_.emplace(
            std::lower_bound(
                channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                *idIt,
                [](IDAndIndex idAndIndex, IDGen::ID id)
                {
                    return idAndIndex.id < id;
                }
            ),
            *idIt, position
        );
        nodeAddedCallback_(*this);
    }
    return ret;
}

bool Mixer::appendChannel(
    Mixer::ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    return insertChannel(
        channelCount(), channelType, channelGroupType, channelCountInGroup);
}

bool Mixer::removeChannel(std::uint32_t first, std::uint32_t removeCount)
{
    if(auto last = first + removeCount;
        first < channelCount() && last <= channelCount())
    {
        preFaderInserts_.erase(
            preFaderInserts_.begin() + first,
            preFaderInserts_.begin() + last
        );
        postFaderInserts_.erase(
            postFaderInserts_.begin() + first,
            postFaderInserts_.begin() + last
        );
        std::vector<ade::NodeHandle> nodesToRemove;
        nodesToRemove.reserve(removeCount * 5);
        FOR_RANGE(i, first,last)
        {
            nodesToRemove.emplace_back(inputDevices_[i].second);
            nodesToRemove.emplace_back(mutes_[i].second);
            nodesToRemove.emplace_back(faders_[i].second);
            nodesToRemove.emplace_back(meters_[i].second);
            nodesToRemove.emplace_back(outputDevices_[i].second);
        }
        FOR_RANGE0(i, nodesToRemove.size())
        {
            graphWithPDC_.removeNode(nodesToRemove[i]);
        }
        nodeRemovedCallback_(*this);
        inputDevices_.erase(
            inputDevices_.begin() + first,
            inputDevices_.begin() + last
        );
        mutes_.erase(
            mutes_.begin() + first,
            mutes_.begin() + last
        );
        meters_.erase(
            meters_.begin() + first,
            meters_.begin() + last
        );
        faders_.erase(
            faders_.begin() + first,
            faders_.begin() + last
        );
        outputDevices_.erase(
            outputDevices_.begin() + first,
            outputDevices_.begin() + last
        );
        channelInfo_.erase(
            channelInfo_.begin() + first,
            channelInfo_.begin() + last
        );
        std::sort(
            channelId_.begin() + first,
            channelId_.begin() + last
        );
        auto it2 = channelId_.begin() + first;
        channelIdAndIndex_.erase(
            std::remove_if(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                [&it2](IDAndIndex idAndIndex)
                {
                    if(idAndIndex.id == *it2)
                    {
                        ++it2;
                        return true;
                    }
                    return false;
                }
            ),
            channelIdAndIndex_.end()
        );
        channelId_.erase(
            channelId_.begin() + first,
            channelId_.begin() + last
        );
        return true;
    }
    return false;
}

void Mixer::clearChannels()
{
    removeChannel(0, channelCount());
}

void Mixer::setNodeAddedCallback(
    NodeAddedCallback* callback)
{
    nodeAddedCallback_ = callback;
}

void Mixer::setNodeRemovedCallback(
    NodeRemovedCallback* callback)
{
    nodeRemovedCallback_ = callback;
}

void Mixer::setConnectionUpdatedCallback(
    ConnectionUpdatedCallback* callback)
{
    connectionUpdatedCallback_ = callback;
}

void Mixer::resetNodeAddedCallback()
{
    nodeAddedCallback_ = &blankNodeAddedCallback;
}

void Mixer::resetNodeRemovedCallback()
{
    nodeRemovedCallback_ = &blankNodeRemovedCallback;
}

void Mixer::resetConnectionUpdatedCallback()
{
    connectionUpdatedCallback_ = &blankConnectionUpdatedCallback;
}

std::optional<ade::EdgeHandle> Mixer::addConnection(
    const ade::NodeHandle& from, const ade::NodeHandle& to,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    auto ret = graph_.connect(from, to, fromChannel, toChannel);
    if(ret.has_value())
    {
        connections_.emplace(*ret);
        connectionUpdatedCallback_(*this);
    }
    return ret;
}

void Mixer::removeConnection(const ade::EdgeHandle& edgeHandle)
{
    if(auto it = connections_.find(edgeHandle); it != connections_.end())
    {
        graph_.disconnect(*it);
        connections_.erase(it);
        connectionUpdatedCallback_(*this);
    }
}

void Mixer::clearConnections()
{
    for(const auto& connection: connections_)
    {
        graph_.disconnect(connection);
    }
    connectionUpdatedCallback_(*this);
    connections_.clear();
}
}