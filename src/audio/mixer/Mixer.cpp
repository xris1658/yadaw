#include "Mixer.hpp"

#include "audio/mixer/BlankGenerator.hpp"
#include "audio/mixer/BlankReceiver.hpp"

namespace YADAW::Audio::Mixer
{
void blankNodeAddedCallback(const Mixer& mixer) {}
void blankNodeRemovedCallback(const Mixer& mixer) {}
void blankConnectionUpdatedCallback(const Mixer& mixer) {}

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

std::optional<bool> Mixer::audioInputMuted(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {audioInputMuted_[index]};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::audioOutputMuted(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {audioOutputMuted_[index]};
    }
    return std::nullopt;
}

std::optional<bool> Mixer::muted(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {muted_[index]};
    }
    return std::nullopt;
}

void Mixer::setAudioInputMuted(std::uint32_t index, bool muted)
{
    if(index < audioInputChannelCount())
    {
        audioInputMutes_[index].first->setMute(muted);
        audioInputMuted_[index] = muted;
    }
}

void Mixer::setAudioOutputMuted(std::uint32_t index, bool muted)
{
    if(index < audioOutputChannelCount())
    {
        audioOutputMutes_[index].first->setMute(muted);
        audioOutputMuted_[index] = muted;
    }
}

void Mixer::setMuted(std::uint32_t index, bool muted)
{
    if(index < channelCount())
    {
        mutes_[index].first->setMute(muted);
        muted_[index] = muted;
    }
}

bool Mixer::appendAudioInputChannel(
    const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex)
{
    return insertAudioInputChannel(audioInputChannelCount(), inNode, channelGroupIndex);
}

bool Mixer::insertAudioInputChannel(
    std::uint32_t position,
    const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex)
{
    auto device = graph_.getNodeData(inNode).process.device();
    if(channelGroupIndex < device->audioOutputGroupCount()
       && position <= audioInputChannelCount())
    {
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
                graph_, inNode, faderNode, channelGroupIndex, 0
            )
        );
        audioInputPostFaderInserts_.emplace(
            audioInputPostFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            )
        );
        graph_.connect(meterNode, muteNode, 0, 0);
        audioInputFaders_.emplace(
            audioInputFaders_.begin() + position,
            std::move(fader), faderNode
        );
        audioInputMeters_.emplace(
            audioInputMeters_.begin() + position,
            std::move(meter), meterNode
        );
        audioInputMutes_.emplace(
            audioInputMutes_.begin() + position,
            std::move(mute), muteNode
        );
        audioInputMuted_.emplace(
            audioInputMuted_.begin() + position,
            false);
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
            nodesToRemove.emplace_back(audioInputFaders_[i].second);
            nodesToRemove.emplace_back(audioInputMeters_[i].second);
            nodesToRemove.emplace_back(audioInputMutes_[i].second);
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
        audioInputFaders_.erase(
            audioInputFaders_.begin() + first,
            audioInputFaders_.begin() + last
        );
        audioInputMeters_.erase(
            audioInputMeters_.begin() + first,
            audioInputMeters_.begin() + last
        );
        audioInputMutes_.erase(
            audioInputMutes_.begin() + first,
            audioInputMutes_.begin() + last
        );
        audioInputMuted_.erase(
            audioInputMuted_.begin() + first,
            audioInputMuted_.begin() + last
        );
        audioInputChannelInfo_.erase(
            audioInputChannelInfo_.begin() + first,
            audioInputChannelInfo_.begin() + last
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
    const ade::NodeHandle& outNode, std::uint32_t channel)
{
    return insertAudioOutputChannel(audioOutputChannelCount(), outNode, channel);
}

bool Mixer::insertAudioOutputChannel(std::uint32_t position, const ade::NodeHandle& outNode, std::uint32_t channel)
{
    // input device of an audio output channel is a summing.
    auto device = graph_.getNodeData(outNode).process.device();
    if(channel < device->audioInputGroupCount()
        && position <= audioOutputChannelCount())
    {
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
                graph_, summingNode, faderNode, 0, 0
            )
        );
        audioOutputPostFaderInserts_.emplace(
            audioOutputPostFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, faderNode, meterNode, 0, 0
            )
        );
        graph_.connect(meterNode, muteNode, 0, 0);
        graph_.connect(muteNode, outNode, 0, channel);
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
        audioOutputMutes_.emplace(
            audioOutputMutes_.begin() + position,
            std::move(mute), muteNode
        );
        audioOutputMuted_.emplace(
            audioOutputMuted_.begin() + position,
            false
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
        nodesToRemove.reserve(removeCount * 4);
        FOR_RANGE(i, first, last)
        {
            nodesToRemove.emplace_back(audioOutputSummings_[i].second);
            nodesToRemove.emplace_back(audioOutputFaders_[i].second);
            nodesToRemove.emplace_back(audioOutputMeters_[i].second);
            nodesToRemove.emplace_back(audioOutputMutes_[i].second);
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
        audioOutputMutes_.erase(
            audioOutputMutes_.begin() + first,
            audioOutputMutes_.begin() + last
        );
        audioOutputMuted_.erase(
            audioOutputMuted_.begin() + first,
            audioOutputMuted_.begin() + last
        );
        audioOutputChannelInfo_.erase(
            audioOutputChannelInfo_.begin() + first,
            audioOutputChannelInfo_.begin() + last
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
                graph_, inputDeviceNode, faderNode, 0, 0
            );
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
            muted_.emplace(muted_.begin() + position, false);
            graph_.connect(meterNode, muteNode, 0, 0);
            outputDevices_.emplace(outputDevices_.begin() + position,
                std::move(outputDevice), outputDeviceNode
            );
            nodeAddedCallback_(*this);
            auto it = channelInfo_.emplace(channelInfo_.begin() + position);
            it->channelType = channelType;
            return true;
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
                graph_, inputDeviceNode, faderNode, 0, 0
            );
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
            muted_.emplace(muted_.begin() + position, false);
            graph_.connect(meterNode, muteNode, 0, 0);
            outputDevices_.emplace(outputDevices_.begin() + position,
                std::move(outputDevice), outputDeviceNode
            );
            nodeAddedCallback_(*this);
            auto it = channelInfo_.emplace(channelInfo_.begin() + position);
            it->channelType = channelType;
            return true;
        }
        }
    }
    return false;
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
            nodesToRemove.emplace_back(faders_[i].second);
            nodesToRemove.emplace_back(meters_[i].second);
            nodesToRemove.emplace_back(outputDevices_[i].second);
            nodesToRemove.emplace_back(mutes_[i].second);
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
        meters_.erase(
            meters_.begin() + first,
            meters_.begin() + last
        );
        faders_.erase(
            faders_.begin() + first,
            faders_.begin() + last
        );
        mutes_.erase(
            mutes_.begin() + first,
            mutes_.begin() + last
        );
        muted_.erase(
            muted_.begin() + first,
            muted_.begin() + last
        );
        outputDevices_.erase(
            outputDevices_.begin() + first,
            outputDevices_.begin() + last
        );
        channelInfo_.erase(
            channelInfo_.begin() + first,
            channelInfo_.begin() + last
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