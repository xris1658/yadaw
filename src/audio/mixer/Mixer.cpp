#include "Mixer.hpp"

#include "audio/mixer/BlankGenerator.hpp"
#include "audio/util/InputSwitcher.hpp"
#include "util/Base.hpp"

namespace YADAW::Audio::Mixer
{
namespace Impl
{
void blankNodeAddedCallback(const Mixer&) {}
void blankNodeRemovedCallback(const Mixer&) {}
void blankConnectionUpdatedCallback(const Mixer&) {}
}

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

OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> Mixer::audioInputChannelPolarityInverterAt(
    std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPolarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> Mixer::audioOutputChannelPolarityInverterAt(
    std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPolarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> Mixer::channelPolarityInverterAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {std::ref(*polarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::PolarityInverter> Mixer::audioInputChannelPolarityInverterAt(std::uint32_t index)
{
    if(index < audioInputChannelCount())
    {
        return {std::ref(*audioInputPolarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::PolarityInverter> Mixer::audioOutputChannelPolarityInverterAt(std::uint32_t index)
{
    if(index < audioOutputChannelCount())
    {
        return {std::ref(*audioOutputPolarityInverters_[index].first)};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::PolarityInverter> Mixer::channelPolarityInverterAt(std::uint32_t index)
{
    if(index < channelCount())
    {
        return {std::ref(*polarityInverters_[index].first)};
    }
    return std::nullopt;
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
Mixer::audioInputChannelGroupTypeAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {audioInputMutes_[index].first->audioInputGroupAt(0)->get().type()};
    }
    return std::nullopt;
}

std::optional<YADAW::Audio::Base::ChannelGroupType> Mixer::audioOutputChannelGroupTypeAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {audioOutputMutes_[index].first->audioInputGroupAt(0)->get().type()};
    }
    return std::nullopt;
}

std::optional<YADAW::Audio::Base::ChannelGroupType> Mixer::channelGroupTypeAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {mutes_[index].first->audioInputGroupAt(0)->get().type()};
    }
    return std::nullopt;
}

std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>>
Mixer::audioInputChannelGroupTypeAndChannelCountAt(std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        auto& channelGroup = audioInputMutes_[index].first->audioInputGroupAt(0)->get();
        return {std::pair(channelGroup.type(), channelGroup.channelCount())};
    }
    return std::nullopt;
}

std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>>
Mixer::audioOutputChannelGroupTypeAndChannelCountAt(std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        auto& channelGroup = audioOutputMutes_[index].first->audioInputGroupAt(0)->get();
        return {std::pair(channelGroup.type(), channelGroup.channelCount())};
    }
    return std::nullopt;
}

std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>>
Mixer::channelGroupTypeAndChannelCountAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        auto& channelGroup = mutes_[index].first->audioInputGroupAt(0)->get();
        return {std::pair(channelGroup.type(), channelGroup.channelCount())};
    }
    return std::nullopt;
}

std::optional<YADAW::Util::AutoIncrementID::ID> Mixer::audioInputChannelIDAt(
    std::uint32_t index) const
{
    if(index < audioInputChannelCount())
    {
        return {audioInputChannelId_[index]};
    }
    return std::nullopt;
}

std::optional<YADAW::Util::AutoIncrementID::ID> Mixer::audioOutputChannelIDAt(
    std::uint32_t index) const
{
    if(index < audioOutputChannelCount())
    {
        return {audioOutputChannelId_[index]};
    }
    return std::nullopt;
}

std::optional<YADAW::Util::AutoIncrementID::ID> Mixer::channelIDAt(
    std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {channelId_[index]};
    }
    return std::nullopt;
}

bool compareIdAndIndexWithId(const Mixer::IDAndIndex& idAndIndex, Mixer::IDGen::ID id)
{
    return idAndIndex.id < id;
}

OptionalRef<const Mixer::Position> Mixer::mainInputAt(std::uint32_t index) const
{
    if(index < channelCount()
        && channelInfo_[index].channelType == ChannelType::Audio)
    {
        return {mainInput_[index]};
    }
    return std::nullopt;
}

bool Mixer::setMainInputAt(std::uint32_t index, Position position)
{
    if(index < channelCount()
        && channelInfo_[index].channelType == ChannelType::Audio)
    {
        // Disconnect
        auto toNode = inputDevices_[index].second;
        auto inEdges = toNode->inEdges();
        if(!inEdges.empty())
        {
            graph_.disconnect(inEdges.front());
        }
        auto ret = false;
        // Connect
        if(position.type == Position::Type::Invalid)
        {
            ret = true;
        }
        if(position.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioInputChannelIdAndIndex_.begin(),
                audioInputChannelIdAndIndex_.end(),
                position.id,
                &compareIdAndIndexWithId
            );
            if(it != audioInputChannelIdAndIndex_.end() && it->id == position.id)
            {
                const auto& fromNode = audioInputPostFaderInserts_[it->index]->outNode();
                ret = graph_.connect(fromNode, toNode, 0, 1).has_value();
            }
        }
        else if(position.type == Position::Type::BusAndFXChannel)
        {
            // not implemented
        }
        else if(position.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        if(ret)
        {
            connectionUpdatedCallback_(*this);
            mainInput_[index] = position;
        }
        return ret;
    }
    return false;
}

OptionalRef<const Mixer::Position> Mixer::mainOutputAt(std::uint32_t index) const
{
    if(index < channelCount())
    {
        return {mainOutput_[index]};
    }
    return std::nullopt;
}

bool Mixer::setMainOutputAt(std::uint32_t index, Position position)
{
    if(index >= channelCount())
    {
        return false;
    }
    auto ret = false;
    const auto& mute = *(mutes_[index].first);
    const auto& channelGroup = mute.audioOutputGroupAt(0)->get();
    auto channelGroupType = channelGroup.type();
    auto channelGroupChannelCount = channelGroup.channelCount();
    // Check if channel config of the destination position matches the source
    // Reconnect the graph
    auto& oldPosition = mainOutput_[index];
    if(oldPosition != position)
    {
        std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC> disconnectingOldMultiInput;
        std::unique_ptr<YADAW::Audio::Device::IAudioDevice> disconnectingOldSumming;
        // Disconnect
        if(oldPosition.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                oldPosition.id,
                &compareIdAndIndexWithId
            );
            if(it != audioOutputChannelIdAndIndex_.end() && it->id == oldPosition.id)
            {
                auto fromNode = postFaderInserts_[index]->outNode();
                auto& [oldSumming, oldSummingNode] = audioOutputSummings_[it->index];
                auto newSumming = std::make_unique<YADAW::Audio::Util::Summing>(
                    oldSumming->audioInputGroupCount() - 1,
                    oldSumming->audioOutputGroupAt(0)->get().type(),
                    oldSumming->audioOutputGroupAt(0)->get().channelCount()
                );
                auto newSummingNode = graphWithPDC_.addNode(
                    YADAW::Audio::Engine::AudioDeviceProcess(*newSumming)
                );
                auto newSummingNodeInIndex = 0U;
                for(const auto& edgeHandle: oldSummingNode->inEdges())
                {
                    if(const auto& srcNode = edgeHandle->srcNode(); srcNode != fromNode)
                    {
                        graph_.connect(
                            srcNode, newSummingNode,
                            graph_.getEdgeData(edgeHandle).fromChannel,
                            newSummingNodeInIndex++
                        );
                    }
                }
                audioOutputPreFaderInserts_[it->index]->setInNode(newSummingNode, 0);
                disconnectingOldMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                disconnectingOldSumming = std::move(oldSumming);
                oldSumming = std::move(newSumming);
                oldSummingNode = newSummingNode;
            }
        }
        else if(oldPosition.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                oldPosition.id,
                &compareIdAndIndexWithId
            );
            if(it != channelIdAndIndex_.end() && it->id == oldPosition.id)
            {
                auto oldType = channelInfo_[it->index].channelType;
                if(oldType == ChannelType::AudioBus || oldType == ChannelType::AudioFX)
                {
                    auto fromNode = postFaderInserts_[index]->outNode();
                    auto& [oldSummingAsDevice, oldSummingNode] = inputDevices_[it->index];
                    auto oldSumming = static_cast<YADAW::Audio::Util::Summing*>(
                        oldSummingAsDevice.get()
                    );
                    auto newSumming = std::make_unique<YADAW::Audio::Util::Summing>(
                        oldSumming->audioInputGroupCount() - 1,
                        oldSumming->audioOutputGroupAt(0)->get().type(),
                        oldSumming->audioOutputGroupAt(0)->get().channelCount()
                    );
                    auto newSummingNode = graphWithPDC_.addNode(
                        YADAW::Audio::Engine::AudioDeviceProcess(*newSumming)
                    );
                    auto newSummingNodeInIndex = 0U;
                    for(const auto& edgeHandle: oldSummingNode->inEdges())
                    {
                        if(const auto& srcNode = edgeHandle->srcNode(); srcNode != fromNode)
                        {
                            graph_.connect(
                                srcNode, newSummingNode,
                                graph_.getEdgeData(edgeHandle).fromChannel,
                                newSummingNodeInIndex++
                            );
                        }
                    }
                    preFaderInserts_[it->index]->setInNode(newSummingNode, 0);
                    disconnectingOldMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                    disconnectingOldSumming = std::move(oldSummingAsDevice);
                    oldSummingAsDevice = std::move(newSumming);
                    oldSummingNode = newSummingNode;
                }
            }
        }
        else if(oldPosition.type == Position::Type::PluginAuxIO)
        {
            // not-implemented
        }
        // Connect
        std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC> disconnectingNewMultiInput;
        std::unique_ptr<YADAW::Audio::Device::IAudioDevice> disconnectingNewSumming;
        if(position.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                position.id,
                &compareIdAndIndexWithId
            );
            if(it != audioOutputChannelIdAndIndex_.end() && it->id == position.id)
            {
                auto outputChannelIndex = it->index;
                const auto& destPair =
                    *audioOutputChannelGroupTypeAndChannelCountAt(outputChannelIndex);
                const auto& srcPair =
                    *channelGroupTypeAndChannelCountAt(index);
                if(srcPair == destPair)
                {
                    auto fromNode = postFaderInserts_[index]->outNode();
                    auto& [oldSumming, oldSummingNode] = audioOutputSummings_[outputChannelIndex];
                    auto newSumming = std::make_unique<YADAW::Audio::Util::Summing>(
                        oldSumming->audioInputGroupCount() + 1,
                        oldSumming->audioOutputGroupAt(0)->get().type(),
                        oldSumming->audioOutputGroupAt(0)->get().channelCount()
                    );
                    auto newSummingNode = graphWithPDC_.addNode(
                        YADAW::Audio::Engine::AudioDeviceProcess(*newSumming)
                    );
                    FOR_RANGE0(i, oldSummingNode->outEdges().size())
                    {
                        graph_.disconnect(oldSummingNode->outEdges().front());
                    }
                    graph_.connect(newSummingNode, audioOutputPolarityInverters_[outputChannelIndex].second, 0, 0);
                    auto newSummingNodeInIndex = 0U;
                    for(const auto& edgeHandle: oldSummingNode->inEdges())
                    {
                        graph_.connect(
                            edgeHandle->srcNode(), newSummingNode,
                            graph_.getEdgeData(edgeHandle).fromChannel,
                            newSummingNodeInIndex++
                        );
                    }
                    graph_.connect(
                        fromNode, newSummingNode, 0, newSummingNodeInIndex
                    );
                    disconnectingNewMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                    disconnectingNewSumming = std::move(oldSumming);
                    connectionUpdatedCallback_(*this);
                    disconnectingOldSumming.reset();
                    disconnectingOldMultiInput.reset();
                    disconnectingNewSumming.reset();
                    disconnectingNewMultiInput.reset();
                    oldSumming = std::move(newSumming);
                    oldSummingNode = newSummingNode;
                    ret = true;
                }
            }
        }
        else if(position.type == Position::Type::BusAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                position.id,
                &compareIdAndIndexWithId
            );
            if(it != channelIdAndIndex_.end() && it->id == position.id)
            {
                auto outputChannelIndex = it->index;
                auto channelType = channelInfo_[outputChannelIndex].channelType;
                if(channelType == ChannelType::AudioBus || channelType == ChannelType::AudioFX)
                {
                    const auto& destPair =
                        *channelGroupTypeAndChannelCountAt(outputChannelIndex);
                    const auto& srcPair =
                        *channelGroupTypeAndChannelCountAt(index);
                    if(srcPair == destPair)
                    {
                        auto fromNode = postFaderInserts_[index]->outNode();
                        auto& [oldSummingAsDevice, oldSummingNode] = inputDevices_[outputChannelIndex];
                        auto oldSumming = static_cast<YADAW::Audio::Util::Summing*>(
                            oldSummingAsDevice.get()
                        );
                        auto newSumming = std::make_unique<YADAW::Audio::Util::Summing>(
                            oldSumming->audioInputGroupCount() + 1,
                            oldSumming->audioOutputGroupAt(0)->get().type(),
                            oldSumming->audioOutputGroupAt(0)->get().channelCount()
                        );
                        auto newSummingNode = graphWithPDC_.addNode(
                            YADAW::Audio::Engine::AudioDeviceProcess(*newSumming)
                        );
                        FOR_RANGE0(i, oldSummingNode->outEdges().size())
                        {
                            graph_.disconnect(oldSummingNode->outEdges().front());
                        }
                        graph_.connect(newSummingNode, audioOutputPolarityInverters_[outputChannelIndex].second, 0, 0);
                        auto newSummingNodeInIndex = 0U;
                        for(const auto& edgeHandle: oldSummingNode->inEdges())
                        {
                            graph_.connect(
                                edgeHandle->srcNode(), newSummingNode,
                                graph_.getEdgeData(edgeHandle).fromChannel,
                                newSummingNodeInIndex++
                            );
                        }
                        graph_.connect(
                            fromNode, newSummingNode, 0, newSummingNodeInIndex
                        );
                        {
                            disconnectingNewMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                            disconnectingNewSumming = std::move(oldSummingAsDevice);
                            oldSummingAsDevice = std::move(newSumming);
                            oldSummingNode = newSummingNode;
                            connectionUpdatedCallback_(*this);
                        }
                        disconnectingOldSumming.reset();
                        disconnectingNewSumming.reset();
                        disconnectingOldMultiInput.reset();
                        disconnectingNewMultiInput.reset();
                        ret = true;
                    }
                }
            }
        }
        else if(position.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        else if(position.type == Position::Type::Invalid)
        {
            connectionUpdatedCallback_(*this);
            disconnectingOldMultiInput.reset();
            disconnectingOldSumming.reset();
            ret = true;
        }
    }
    if(ret)
    {
        oldPosition = position;
    }
    return ret;
}

std::optional<std::uint32_t> Mixer::getInputIndexOfId(IDGen::ID id) const
{
    auto it = std::lower_bound(
        audioInputChannelIdAndIndex_.begin(),
        audioInputChannelIdAndIndex_.end(),
        id,
        &compareIdAndIndexWithId
    );
    if(it != audioInputChannelIdAndIndex_.end() && it->id == id)
    {
        return {it->index};
    }
    return std::nullopt;
}

std::optional<std::uint32_t> Mixer::getOutputIndexOfId(IDGen::ID id) const
{
    auto it = std::lower_bound(
        audioOutputChannelIdAndIndex_.begin(),
        audioOutputChannelIdAndIndex_.end(),
        id,
        &compareIdAndIndexWithId
    );
    if(it != audioOutputChannelIdAndIndex_.end() && it->id == id)
    {
        return {it->index};
    }
    return std::nullopt;
}

std::optional<std::uint32_t> Mixer::getIndexOfId(IDGen::ID id) const
{
    auto it = std::lower_bound(
        channelIdAndIndex_.begin(),
        channelIdAndIndex_.end(),
        id,
        &compareIdAndIndexWithId
    );
    if(it != channelIdAndIndex_.end() && it->id == id)
    {
        return {it->index};
    }
    return std::nullopt;
}

bool Mixer::hasMuteInAudioInputChannels() const
{
    auto range = YADAW::Util::IntegerRange(audioInputChannelCount());
    return std::any_of(
        range.begin(), range.end(),
        [this](std::uint32_t index)
        {
            return audioInputMutes_[index].first->getMute();
        }
    );
}

bool Mixer::hasMuteInRegularChannels() const
{
    auto range = YADAW::Util::IntegerRange(channelCount());
    return std::any_of(
        range.begin(), range.end(),
        [this](std::uint32_t index)
        {
            return mutes_[index].first->getMute();
        }
    );
}

bool Mixer::hasMuteInAudioOutputChannels() const
{
    auto range = YADAW::Util::IntegerRange(audioOutputChannelCount());
    return std::any_of(
        range.begin(), range.end(),
        [this](std::uint32_t index)
        {
            return audioOutputMutes_[index].first->getMute();
        }
    );
}

void Mixer::unmuteAudioInputChannels()
{
    FOR_RANGE0(i, audioInputChannelCount())
    {
        audioInputMutes_[i].first->setMute(false);
    }
}

void Mixer::unmuteRegularChannels()
{
    FOR_RANGE0(i, channelCount())
    {
        mutes_[i].first->setMute(false);
    }
}

void Mixer::unmuteAudioOutputChannels()
{
    FOR_RANGE0(i, audioOutputChannelCount())
    {
        audioOutputMutes_[i].first->setMute(false);
    }
}

void Mixer::unmuteAllChannels()
{
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
        auto meter = meterFactory_(channelGroup.type(), channelGroup.channelCount());
        auto meterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*meter)
        );
        auto fader = volumeFaderFactory_(channelGroup.type(), channelGroup.channelCount());
        auto faderNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*fader)
        );
        auto mute = std::make_unique<YADAW::Audio::Util::Mute>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto muteNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*mute)
        );
        auto polarityInverter = std::make_unique<YADAW::Audio::Mixer::PolarityInverter>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto polarityInverterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*polarityInverter)
        );
        graph_.connect(inNode, polarityInverterNode, 0, 0);
        audioInputPreFaderInserts_.emplace(
            audioInputPreFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, polarityInverterNode, muteNode, channelGroupIndex, 0
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
        audioInputPolarityInverters_.emplace(
            audioInputPolarityInverters_.begin() + position,
            std::move(polarityInverter), polarityInverterNode
        );
        audioInputFaders_.emplace(
            audioInputFaders_.begin() + position,
            std::move(fader), faderNode
        );
        audioInputMeters_.emplace(
            audioInputMeters_.begin() + position,
            std::move(meter), meterNode
        );
        nodeRemovedCallback_(*this);
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
        {
            std::vector<ade::NodeHandle> nodesToRemove;
            nodesToRemove.reserve(removeCount * 5);
            FOR_RANGE(i, first, last)
            {
                auto polarityInverterNode = audioInputPolarityInverters_[i].second;
                nodesToRemove.emplace_back(polarityInverterNode);
                nodesToRemove.emplace_back(polarityInverterNode->inNodes().front());
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
        audioInputPolarityInverters_.erase(
            audioInputPolarityInverters_.begin() + first,
            audioInputPolarityInverters_.begin() + last
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
        auto meter = meterFactory_(channelGroup.type(), channelGroup.channelCount());
        auto meterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*meter)
        );
        auto fader = volumeFaderFactory_(channelGroup.type(), channelGroup.channelCount());
        auto faderNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*fader)
        );
        auto mute = std::make_unique<YADAW::Audio::Util::Mute>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto muteNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*mute)
        );
        auto polarityInverter = std::make_unique<YADAW::Audio::Mixer::PolarityInverter>(
            channelGroup.type(), channelGroup.channelCount()
        );
        auto polarityInverterNode = graph_.addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(*polarityInverter)
        );
        audioOutputPreFaderInserts_.emplace(
            audioOutputPreFaderInserts_.begin() + position,
            std::make_unique<YADAW::Audio::Mixer::Inserts>(
                graph_, polarityInverterNode, muteNode, 0, 0
            )
        );
        graph_.connect(summingNode, polarityInverterNode, 0, 0);
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
        audioOutputPolarityInverters_.emplace(
            audioOutputPolarityInverters_.begin() + position,
            std::move(polarityInverter), polarityInverterNode
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
        {
            std::vector<ade::NodeHandle> nodesToRemove;
            nodesToRemove.reserve(removeCount * 6);
            FOR_RANGE(i, first, last)
            {
                nodesToRemove.emplace_back(audioOutputSummings_[i].second);
                nodesToRemove.emplace_back(audioOutputMutes_[i].second);
                nodesToRemove.emplace_back(audioOutputFaders_[i].second);
                nodesToRemove.emplace_back(audioOutputMeters_[i].second);
                nodesToRemove.emplace_back(
                    *(audioOutputMeters_[i].second->outNodes().begin())
                );
                nodesToRemove.emplace_back(
                    audioOutputPolarityInverters_[i].second
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

bool Mixer::insertChannels(
    std::uint32_t position, std::uint32_t count,
    ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    std::vector<DeviceAndNode> inputDevicesAndNode;
    std::vector<FaderAndNode> fadersAndNode;
    std::vector<MeterAndNode> metersAndNode;
    std::vector<MuteAndNode> mutesAndNode;
    std::vector<PolarityInverterAndNode> polarityInvertersAndNode;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> preFaderInserts;
    std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> postFaderInserts;
    auto ret = false;
    if(position <= channelCount() && count > 0)
    {
        // inputs ----------------------------------------------------------
        inputDevicesAndNode.reserve(count);
        // polarity inverters -----------------------------------------------
        polarityInvertersAndNode.reserve(count);
        FOR_RANGE0(i, count)
        {
            auto& [device, node] = polarityInvertersAndNode.emplace_back(
                std::make_unique<YADAW::Audio::Mixer::PolarityInverter>(
                    channelGroupType, channelCountInGroup
                ),
                ade::NodeHandle()
            );
            node = graph_.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(*device)
            );
        }
        // faders ----------------------------------------------------------
        fadersAndNode.reserve(count);
        FOR_RANGE0(i, count)
        {
            auto& faderAndNode =  fadersAndNode.emplace_back(
                volumeFaderFactory_(channelGroupType, channelCountInGroup),
                ade::NodeHandle()
            );
            faderAndNode.second = graph_.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(
                    *(faderAndNode.first)
                )
            );
        }
        // meters ----------------------------------------------------------
        metersAndNode.reserve(count);
        FOR_RANGE0(i, count)
        {
            auto& meterAndNode = metersAndNode.emplace_back(
                meterFactory_(channelGroupType, channelCountInGroup),
                ade::NodeHandle()
            );
            meterAndNode.second = graph_.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(
                    *(meterAndNode.first)
                )
            );
        }
        // mutes -----------------------------------------------------------
        mutesAndNode.reserve(count);
        FOR_RANGE0(i, count)
        {
            auto& muteAndNode = mutesAndNode.emplace_back(
                std::make_unique<YADAW::Audio::Util::Mute>(
                    channelGroupType, channelCountInGroup
                ),
                ade::NodeHandle()
            );
            muteAndNode.second = graph_.addNode(
                YADAW::Audio::Engine::AudioDeviceProcess(
                    *(muteAndNode.first)
                )
            );
        }
        // post-fader inserts ----------------------------------------------
        postFaderInserts.reserve(count);
        FOR_RANGE0(i, count)
        {
            postFaderInserts.emplace_back(
                std::make_unique<YADAW::Audio::Mixer::Inserts>(
                    graph_,
                    fadersAndNode[i].second, metersAndNode[i].second, 0, 0
                )
            );
        }
        switch(channelType)
        {
        case ChannelType::Audio:
        {
            // inputs ----------------------------------------------------------
            std::vector<std::unique_ptr<YADAW::Audio::Util::InputSwitcher>> inputSwitchers;
            inputSwitchers.reserve(count);
            FOR_RANGE0(i, count)
            {
                inputSwitchers.emplace_back(
                    std::make_unique<YADAW::Audio::Util::InputSwitcher>(
                        2, channelGroupType, channelCountInGroup
                    )
                );
            }
            FOR_RANGE0(i, count)
            {
                auto node = graph_.addNode(
                    YADAW::Audio::Engine::AudioDeviceProcess(
                        *inputSwitchers[i]
                    )
                );
                inputDevicesAndNode.emplace_back(std::move(inputSwitchers[i]), node);
            }
            ret = true;
            break;
        }
        case ChannelType::Instrument:
        {
            std::generate_n(
                std::back_inserter(inputDevicesAndNode),
                count,
                []()
                {
                    return DeviceAndNode{
                        std::unique_ptr<YADAW::Audio::Device::IAudioDevice>(),
                        ade::NodeHandle()
                    };
                }
            );
            ret = true;
            break;
        }
        case ChannelType::AudioFX:
        case ChannelType::AudioBus:
        {
            // inputs ----------------------------------------------------------
            inputDevicesAndNode.reserve(count);
            {
                std::vector<std::unique_ptr<YADAW::Audio::Util::Summing>> summings;
                summings.reserve(count);
                FOR_RANGE0(i, count)
                {
                    summings.emplace_back(
                        std::make_unique<YADAW::Audio::Util::Summing>(
                            0, channelGroupType, channelCountInGroup
                        )
                    );
                }
                FOR_RANGE0(i, count)
                {
                    auto node = graph_.addNode(
                        YADAW::Audio::Engine::AudioDeviceProcess(
                            *summings[i]
                        )
                    );
                    inputDevicesAndNode.emplace_back(summings[i].release(), node);
                }
            }
            ret = true;
            break;
        }
        }
    }
    if(ret)
    {
        // pre-fader inserts -----------------------------------------------
        std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> preFaderInserts;
        preFaderInserts.reserve(count);
        FOR_RANGE0(i, count)
        {
            preFaderInserts.emplace_back(
                std::make_unique<YADAW::Audio::Mixer::Inserts>(
                    graph_,
                    polarityInvertersAndNode[i].second, mutesAndNode[i].second,
                    0, 0
                )
            );
        }
        // connect and move-------------------------------------------------
        if(channelType != ChannelType::Instrument)
        {
            FOR_RANGE0(i, count)
            {
                graph_.connect(
                    inputDevicesAndNode[i].second, polarityInvertersAndNode[i].second, 0, 0
                );
            }
        }
        FOR_RANGE0(i, count)
        {
            graph_.connect(
                mutesAndNode[i].second, fadersAndNode[i].second, 0, 0
            );
        }
        std::move(
            inputDevicesAndNode.begin(), inputDevicesAndNode.end(),
            std::inserter(
                inputDevices_, inputDevices_.begin() + position
            )
        );
        std::move(
            polarityInvertersAndNode.begin(), polarityInvertersAndNode.end(),
            std::inserter(
                polarityInverters_, polarityInverters_.begin() + position
            )
        );
        std::move(
            preFaderInserts.begin(), preFaderInserts.end(),
            std::inserter(
                preFaderInserts_, preFaderInserts_.begin() + position
            )
        );
        std::move(
            postFaderInserts.begin(), postFaderInserts.end(),
            std::inserter(
                postFaderInserts_, postFaderInserts_.begin() + position
            )
        );
        std::move(
            metersAndNode.begin(), metersAndNode.end(),
            std::inserter(
                meters_, meters_.begin() + position
            )
        );
        std::move(
            fadersAndNode.begin(), fadersAndNode.end(),
            std::inserter(
                faders_, faders_.begin() + position
            )
        );
        std::move(
            mutesAndNode.begin(), mutesAndNode.end(),
            std::inserter(
                mutes_, mutes_.begin() + position
            )
        );
        // info, etc.
        std::fill_n(
            std::inserter(channelInfo_, channelInfo_.begin() + position), count,
            ChannelInfo {QString(), QColor(), channelType}
        );
        std::generate_n(
            std::inserter(channelId_, channelId_.begin() + position), count,
            [this]()
            {
                return channelIdGen_();
            }
        );
        for(auto& [id, index]: channelIdAndIndex_)
        {
            if(index >= position)
            {
                index += count;
            }
        }
        FOR_RANGE0(i, count)
        {
            channelIdAndIndex_.emplace(
                std::lower_bound(
                    channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                    channelId_[position + i],
                    [](IDAndIndex idAndIndex, IDGen::ID id)
                    {
                        return idAndIndex.id < id;
                    }
                ),
                channelId_[position + i], position + i
            );
        }
        std::fill_n(
            std::inserter(mainInput_, mainInput_.begin() + position), count,
            Position {
                Position::Invalid, 0, IDGen::InvalidId
            }
        );
        std::fill_n(
            std::inserter(mainOutput_, mainOutput_.begin() + position), count,
            Position {Position::Invalid, 0, IDGen::InvalidId}
        );
        nodeAddedCallback_(*this);
    }
    return ret;
}

bool Mixer::appendChannels(
    std::uint32_t count, ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    return insertChannels(
        channelCount(), count, channelType, channelGroupType, channelCountInGroup);
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
        FOR_RANGE(i, first, last)
        {
            if(channelInfo_[i].channelType != ChannelType::Instrument)
            {
                nodesToRemove.emplace_back(inputDevices_[i].second);
            }
            nodesToRemove.emplace_back(polarityInverters_[i].second);
            nodesToRemove.emplace_back(mutes_[i].second);
            nodesToRemove.emplace_back(faders_[i].second);
            nodesToRemove.emplace_back(meters_[i].second);
        }
        FOR_RANGE0(i, nodesToRemove.size())
        {
            graphWithPDC_.removeNode(nodesToRemove[i]);
        }
        nodeRemovedCallback_(*this);
        mainOutput_.erase(
            mainOutput_.begin() + first,
            mainOutput_.begin() + last
        );
        mainInput_.erase(
            mainInput_.begin() + first,
            mainInput_.begin() + last
        );
        inputDevices_.erase(
            inputDevices_.begin() + first,
            inputDevices_.begin() + last
        );
        polarityInverters_.erase(
            polarityInverters_.begin() + first,
            polarityInverters_.begin() + last
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
                [this, first, last](IDAndIndex idAndIndex)
                {
                    auto it = std::lower_bound(
                        channelId_.begin() + first,
                        channelId_.begin() + last,
                        idAndIndex.id
                    );
                    return it != channelId_.begin() + last
                        && *it == idAndIndex.id;
                }
            ),
            channelIdAndIndex_.end()
        );
        for(auto& [id, index]: channelIdAndIndex_)
        {
            if(index >= last)
            {
                index -= removeCount;
            }
        }
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
    nodeAddedCallback_ = &Impl::blankNodeAddedCallback;
}

void Mixer::resetNodeRemovedCallback()
{
    nodeRemovedCallback_ = &Impl::blankNodeRemovedCallback;
}

void Mixer::resetConnectionUpdatedCallback()
{
    connectionUpdatedCallback_ = &Impl::blankConnectionUpdatedCallback;
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