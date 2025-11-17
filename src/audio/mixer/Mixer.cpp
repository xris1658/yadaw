#include "Mixer.hpp"

#include "audio/util/InputSwitcher.hpp"
#include "util/Base.hpp"

#include <algorithm>
#include <ranges>

namespace YADAW::Audio::Mixer
{
namespace Impl
{
void blankConnectionUpdatedCallback(const Mixer&) {}

void blankPreInsertChannelCallback(const Mixer&, Mixer::PreInsertChannelCallbackArgs) {}

void blankSendAddedCallback(const Mixer& sender, const Mixer::SendPosition& sendPosition) {}

void blankSendDestinationChangedCallback(const Mixer& sender, const Mixer::SendPosition& sendPosition) {}

void blankSendRemovedCallback(const Mixer& sender, Mixer::SendRemovedCallbackArgs args) {}

void blankAuxInputChangedCallback(const Mixer& sender, const Mixer::PluginAuxIOPosition& auxInput) {}

void blankAuxOutputAddedCallback(const Mixer& sender, Mixer::AuxOutputAddedCallbackArgs args) {}

void blankAuxOutputDestinationChangedCallback(const Mixer& sender, Mixer::AuxOutputDestinationChangedCallbackArgs args) {}

void blankAuxOutputRemovedCallback(const Mixer& sender, Mixer::AuxOutputRemovedCallbackArgs args) {}
}

struct InsertPosition
{
    Mixer& mixer;
    Mixer::ChannelListType type;
    std::uint32_t channelIndex;
    std::uint32_t insertsIndex;
};

bool compareIdAndIndexWithId(const Mixer::IDAndIndex& idAndIndex, IDGen::ID id)
{
    return idAndIndex.id < id;
}

Mixer::Mixer():
    graph_(),
    graphWithPDC_(graph_),
    connectionUpdatedCallback_(&Impl::blankConnectionUpdatedCallback),
    preInsertChannelCallback_{
        &Impl::blankPreInsertChannelCallback,
        &Impl::blankPreInsertChannelCallback,
        &Impl::blankPreInsertChannelCallback
    },
    sendAddedCallback_(&Impl::blankSendAddedCallback),
    sendDestinationChangedCallback_(&Impl::blankSendDestinationChangedCallback),
    sendRemovedCallback_(&Impl::blankSendRemovedCallback),
    auxInputChangedCallback_(&Impl::blankAuxInputChangedCallback),
    auxOutputAddedCallback_(&Impl::blankAuxOutputAddedCallback),
    auxOutputDestinationChangedCallback_(Impl::blankAuxOutputDestinationChangedCallback),
    auxOutputRemovedCallback_(&Impl::blankAuxOutputRemovedCallback)
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

const YADAW::Audio::Engine::Extension::NameTag& Mixer::nameTagExtension() const
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::NameTag>();
}

YADAW::Audio::Engine::Extension::NameTag& Mixer::nameTag()
{
    return graph_.getExtension<YADAW::Audio::Engine::Extension::NameTag>();
}

std::uint32_t Mixer::count(ChannelListType type) const
{
    return channelFaders_[type].size();
}

OptionalRef<const YADAW::Audio::Mixer::PolarityInverter> Mixer::polarityInverterAt(
    ChannelListType type, std::uint32_t index) const
{
    if(auto& polarityInverters = channelPolarityInverters_[type];
        index < polarityInverters.size())
    {
        return {*polarityInverters[index].first};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::PolarityInverter> Mixer::polarityInverterAt(
    ChannelListType type, std::uint32_t index)
{
    if(auto& polarityInverters = channelPolarityInverters_[type];
        index < polarityInverters.size())
    {
        return {*polarityInverters[index].first};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts> Mixer::preFaderInsertsAt(
    ChannelListType type, std::uint32_t index) const
{
    if(auto& inserts = channelPreFaderInserts_[type]; index < inserts.size())
    {
        return {*inserts[index]};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts> Mixer::preFaderInsertsAt(
    ChannelListType type, std::uint32_t index)
{
    if(auto& inserts = channelPreFaderInserts_[type]; index < inserts.size())
    {
        return {*inserts[index]};
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Mixer::Inserts> Mixer::postFaderInsertsAt(
    ChannelListType type, std::uint32_t index) const
{
    if(auto& inserts = channelPostFaderInserts_[type]; index < inserts.size())
    {
        return {*inserts[index]};
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Mixer::Inserts> Mixer::postFaderInsertsAt(
    ChannelListType type, std::uint32_t index)
{
    if(auto& inserts = channelPostFaderInserts_[type]; index < inserts.size())
    {
        return {*inserts[index]};
    }
    return std::nullopt;
}

std::optional<const std::uint32_t> Mixer::sendCount(ChannelListType type, std::uint32_t index) const
{
    if(index < count(type))
    {
        return channelSendDestinations_[type][index].size();
    }
    return std::nullopt;
}

std::optional<const bool> Mixer::sendIsPreFader(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(auto& sendPolarityInverters = channelSendPolarityInverters_[type]; channelIndex < sendPolarityInverters.size())
    {
        if(auto& polarityInverters = sendPolarityInverters[channelIndex]; sendIndex < polarityInverters.size())
        {
            return polarityInverters[sendIndex].second->inNodes().front()
                == channelMutes_[type][channelIndex].second;
        }
    }
    return std::nullopt;
}

std::optional<const Mixer::Position> Mixer::sendDestination(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(auto& sendDestinations = channelSendDestinations_[type]; channelIndex < sendDestinations.size())
    {
        if(auto& destinations = sendDestinations[channelIndex]; sendIndex < destinations.size())
        {
            return destinations[sendIndex];
        }
    }
    return std::nullopt;
}

std::optional<const IDGen::ID> Mixer::sendID(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(auto& sendIDs = channelSendIDs_[type]; channelIndex < sendIDs.size())
    {
        if(auto& ids = sendIDs[channelIndex]; sendIndex < ids.size())
        {
            return ids[sendIndex]->first;
        }
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::sendFaderAt(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(auto& sendFaders = channelSendFaders_[type]; channelIndex < sendFaders.size())
    {
        if(auto& faders = sendFaders[channelIndex]; sendIndex < faders.size())
        {
            return *faders[sendIndex].first;
        }
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::sendFaderAt(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(auto& sendFaders = channelSendFaders_[type]; channelIndex < sendFaders.size())
    {
        if(auto& faders = sendFaders[channelIndex]; sendIndex < faders.size())
        {
            return *faders[sendIndex].first;
        }
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::sendMuteAt(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(auto& sendMutes = channelSendMutes_[type]; channelIndex < sendMutes.size())
    {
        if(auto& mutes = sendMutes[channelIndex]; sendIndex < mutes.size())
        {
            return *mutes[sendIndex].first;
        }
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::sendMuteAt(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(auto& sendMutes = channelSendMutes_[type]; channelIndex < sendMutes.size())
    {
        if(auto& mutes = sendMutes[channelIndex]; sendIndex < mutes.size())
        {
            return *mutes[sendIndex].first;
        }
    }
    return std::nullopt;
}

OptionalRef<const PolarityInverter> Mixer::sendPolarityInverterAt(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex) const
{
    if(auto& sendPolarityInverters = channelSendPolarityInverters_[type]; channelIndex < sendPolarityInverters.size())
    {
        if(auto& polarityInverters = sendPolarityInverters[channelIndex]; sendIndex < polarityInverters.size())
        {
            return *polarityInverters[sendIndex].first;
        }
    }
    return std::nullopt;
}

OptionalRef<PolarityInverter> Mixer::sendPolarityInverterAt(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex)
{
    if(auto& sendPolarityInverters = channelSendPolarityInverters_[type]; channelIndex < sendPolarityInverters.size())
    {
        if(auto& polarityInverters = sendPolarityInverters[channelIndex]; sendIndex < polarityInverters.size())
        {
            return *polarityInverters[sendIndex].first;
        }
    }
    return std::nullopt;
}

OptionalRef<const Mixer::ChannelInfo> Mixer::channelInfoAt(ChannelListType type, std::uint32_t index) const
{
    if(auto& channelInfos = channelInfos_[type]; index < channelInfos.size())
    {
        return channelInfos[index];
    }
    return std::nullopt;
}

OptionalRef<Mixer::ChannelInfo> Mixer::channelInfoAt(ChannelListType type, std::uint32_t index)
{
    if(auto& channelInfos = channelInfos_[type]; index < channelInfos.size())
    {
        return channelInfos[index];
    }
    return std::nullopt;
}

OptionalRef<const VolumeFader> Mixer::volumeFaderAt(ChannelListType type, std::uint32_t index) const
{
    if(auto& faders = channelFaders_[type]; index < faders.size())
    {
        return *faders[index].first;
    }
    return std::nullopt;
}

OptionalRef<VolumeFader> Mixer::volumeFaderAt(ChannelListType type, std::uint32_t index)
{
    if(auto& faders = channelFaders_[type]; index < faders.size())
    {
        return *faders[index].first;
    }
    return std::nullopt;
}

OptionalRef<const YADAW::Audio::Util::Mute> Mixer::muteAt(ChannelListType type, std::uint32_t index) const
{
    if(auto& mutes = channelMutes_[type]; index < mutes.size())
    {
        return *mutes[index].first;
    }
    return std::nullopt;
}

OptionalRef<YADAW::Audio::Util::Mute> Mixer::muteAt(ChannelListType type, std::uint32_t index)
{
    if(auto& mutes = channelMutes_[type]; index < mutes.size())
    {
        return *mutes[index].first;
    }
    return std::nullopt;
}

OptionalRef<const Meter> Mixer::meterAt(ChannelListType type, std::uint32_t index) const
{
    if(auto& meters = channelMeters_[type]; index < meters.size())
    {
        return *meters[index].first;
    }
    return std::nullopt;
}

OptionalRef<Meter> Mixer::meterAt(ChannelListType type, std::uint32_t index)
{
    if(auto& meters = channelMeters_[type]; index < meters.size())
    {
        return *meters[index].first;
    }
    return std::nullopt;
}

std::optional<std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t>> Mixer::channelGroupTypeAndChannelCountAt(
    ChannelListType type, std::uint32_t index) const
{
    if(auto& mutes = channelMutes_[type]; index < mutes.size())
    {
        auto& channelGroup = mutes[index].first->audioInputGroupAt(0)->get();
        return {std::pair(channelGroup.type(), channelGroup.channelCount())};
    }
    return std::nullopt;
}

std::optional<IDGen::ID> Mixer::idAt(ChannelListType type, std::uint32_t index) const
{
    if(auto& ids = channelIDs_[type]; index < ids.size())
    {
        return ids[index];
    }
    return std::nullopt;
}

std::optional<std::uint32_t> Mixer::getChannelIndexOfId(ChannelListType type, IDGen::ID id) const
{
    auto& idAndIndex = channelIDAndIndex_[type];
    auto it = std::lower_bound(
        idAndIndex.begin(), idAndIndex.end(), id, &compareIdAndIndexWithId
    );
    if(it != idAndIndex.end() && it->id == id)
    {
        return {it->index};
    }
    return std::nullopt;
}

bool Mixer::hasMute(ChannelListType type) const
{
    return std::ranges::any_of(
        channelMutes_[type],
       [](const MuteAndNode& muteAndNode)
       {
           return muteAndNode.first->getMute();
       }
    );
}

void Mixer::unmute(ChannelListType type)
{
    std::ranges::for_each(
        channelMutes_[type],
        [](MuteAndNode& muteAndNode)
        {
            muteAndNode.first->setMute(false);
        }
    );
}

void Mixer::unmute()
{
    unmute(ChannelListType::AudioHardwareInputList);
    unmute(ChannelListType::RegularList);
    unmute(ChannelListType::AudioHardwareOutputList);
}

bool Mixer::remove(ChannelListType type, std::uint32_t index, std::uint32_t removeCount)
{
    if(auto last = index + removeCount; index < count(type) && index + removeCount <= count(type))
    {
        std::vector<ade::NodeHandle> nodesToRemove;
        nodesToRemove.reserve(
            removeCount * (type == ChannelListType::AudioHardwareOutputList? 6: 5)
        );
        if(type == ChannelListType::AudioHardwareInputList)
        {
            FOR_RANGE(i, index, last)
            {
                nodesToRemove.emplace_back(
                    channelPolarityInverters_[type][i].second->inNodes().front()
                );
            }
        }
        else if(type == ChannelListType::RegularList)
        {
            instrumentBypassed_.erase(
                instrumentBypassed_.begin() + index,
                instrumentBypassed_.begin() + last
            );
            instrumentOutputChannelIndex_.erase(
                instrumentOutputChannelIndex_.begin() + index,
                instrumentOutputChannelIndex_.begin() + last
            );
            FOR_RANGE(i, index, last)
            {
                if(auto node = inputDevices_[i].second; node != nullptr)
                {
                    nodesToRemove.emplace_back(node);
                }
            }
        }
        else if(type == ChannelListType::AudioHardwareOutputList)
        {
            FOR_RANGE(i, index, last)
            {
                nodesToRemove.emplace_back(
                    audioOutputSummings_[i].second
                );
                nodesToRemove.emplace_back(
                    channelMeters_[type][i].second->outNodes().front()
                );
            }
        }
        channelPreFaderInserts_[type].erase(
            channelPreFaderInserts_[type].begin() + index,
            channelPreFaderInserts_[type].begin() + last
        );
        channelPostFaderInserts_[type].erase(
            channelPostFaderInserts_[type].begin() + index,
            channelPostFaderInserts_[type].begin() + last
        );
        FOR_RANGE(i, index, last)
        {
            nodesToRemove.emplace_back(channelPolarityInverters_[type][i].second);
            nodesToRemove.emplace_back(channelMutes_[type][i].second);
            nodesToRemove.emplace_back(channelFaders_[type][i].second);
            nodesToRemove.emplace_back(channelMeters_[type][i].second);
        }
        FOR_RANGE(i, index, last)
        {
            clearSends(type, i);
        }
        if(batchUpdater_)
        {
            for(const auto& node: nodesToRemove)
            {
                if(auto device = graphWithPDC_.removeNode(node))
                {
                    batchUpdater_->addObject(std::move(device));
                }
            }
        }
        else
        {
            std::vector<std::unique_ptr<YADAW::Audio::Engine::MultiInputDeviceWithPDC>> devicesToRemove;
            devicesToRemove.reserve(nodesToRemove.size());
            for(const auto& node: nodesToRemove)
            {
                if(auto device = graphWithPDC_.removeNode(node))
                {
                    devicesToRemove.emplace_back(std::move(device));
                }
            }
            connectionUpdatedCallback_(*this);
        }
        if(type == ChannelListType::RegularList)
        {
            mainOutput_.erase(
                mainOutput_.begin() + index,
                mainOutput_.begin() + last
            );
            mainInput_.erase(
                mainInput_.begin() + index,
                mainInput_.begin() + last
            );
            inputDevices_.erase(
                inputDevices_.begin() + index,
                inputDevices_.begin() + last
            );
            instrumentContexts_.erase(
                instrumentContexts_.begin() + index,
                instrumentContexts_.begin() + last
            );
        }
        channelPolarityInverters_[type].erase(
            channelPolarityInverters_[type].begin() + index,
            channelPolarityInverters_[type].begin() + last
        );
        channelMutes_[type].erase(
            channelMutes_[type].begin() + index,
            channelMutes_[type].begin() + last
        );
        channelMeters_[type].erase(
            channelMeters_[type].begin() + index,
            channelMeters_[type].begin() + last
        );
        channelFaders_[type].erase(
            channelFaders_[type].begin() + index,
            channelFaders_[type].begin() + last
        );
        channelSendIDs_[type].erase(
            channelSendIDs_[type].begin() + index,
            channelSendIDs_[type].begin() + last
        );
        FOR_RANGE(i, index, channelSendIDs_[type].size())
        {
            for(const auto& it: channelSendIDs_[type][i])
            {
                it->second.channelIndex -= removeCount;
            }
        }
        channelSendPolarityInverters_[type].erase(
            channelSendPolarityInverters_[type].begin() + index,
            channelSendPolarityInverters_[type].begin() + last
        );
        channelSendMutes_[type].erase(
            channelSendMutes_[type].begin() + index,
            channelSendMutes_[type].begin() + last
        );
        channelSendFaders_[type].erase(
            channelSendFaders_[type].begin() + index,
            channelSendFaders_[type].begin() + last
        );
        channelSendDestinations_[type].erase(
            channelSendDestinations_[type].begin() + index,
            channelSendDestinations_[type].begin() + last
        );
        channelInfos_[type].erase(
            channelInfos_[type].begin() + index,
            channelInfos_[type].begin() + last
        );
        std::sort(
            channelIDs_[type].begin() + index,
            channelIDs_[type].begin() + last
        );
        auto it2 = channelIDs_[type].begin() + index;
        std::erase_if(
            channelIDAndIndex_[type],
            [this, index, last, type](IDAndIndex idAndIndex)
            {
                auto it = std::lower_bound(
                    channelIDs_[type].begin() + index,
                    channelIDs_[type].begin() + last,
                    idAndIndex.id
                );
                return it != channelIDs_[type].begin() + last
                    && *it == idAndIndex.id;
            }
        );
        for(auto& [id, index]: channelIDAndIndex_[type])
        {
            if(index >= last)
            {
                index -= removeCount;
            }
        }
        channelIDs_[type].erase(
            channelIDs_[type].begin() + index,
            channelIDs_[type].begin() + last
        );
        auto& vecInput = pluginAuxInputs_[type];
        vecInput.erase(vecInput.begin() + index, vecInput.begin() + last);
        auto& vecOutput = pluginAuxOutputs_[type];
        vecOutput.erase(vecOutput.begin() + index, vecOutput.begin() + last);
        auto& vecInputSources = pluginAuxInputSources_[type];
        vecInputSources.erase(vecInputSources.begin() + index, vecInputSources.begin() + last);
        auto& vecOutputDestinations = pluginAuxOutputDestinations_[type];
        vecOutputDestinations.erase(vecOutputDestinations.begin() + index, vecOutputDestinations.begin() + last);
        updatePluginAuxPosition(type, index);
        return true;
    }
    return false;
}

void Mixer::clear(ChannelListType type)
{
    remove(type, 0, count(type));
}

void Mixer::clear()
{
    clear(ChannelListType::AudioHardwareInputList);
    clear(ChannelListType::RegularList);
    clear(ChannelListType::AudioHardwareOutputList);
}

std::optional<bool> Mixer::appendSend(ChannelListType type,
    std::uint32_t channelIndex, bool isPreFader, Position destination)
{
    return insertSend(type, channelIndex, *sendCount(type, channelIndex), isPreFader, destination);
}

std::optional<bool> Mixer::insertSend(ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendPosition,
    bool isPreFader, Position destination)
{
    if(channelIndex < count(type)
        && sendPosition <= *sendCount(type, channelIndex))
    {
        auto& idAndIndex = channelIDAndIndex_[type];
        if(destination.type == Position::Type::AudioHardwareIOChannel)
        {
            if(auto& dests = channelSendDestinations_[type][channelIndex];
                std::ranges::find(dests, destination) == dests.end())
            {
                auto it = std::lower_bound(
                    idAndIndex.begin(), idAndIndex.end(), destination.id
                );
                if(it != idAndIndex.end() && it->id == destination.id)
                {
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    auto fromNode = isPreFader?
                        channelMutes_[type][channelIndex].second:
                        channelFaders_[type][channelIndex].second;
                    if(!YADAW::Util::pathExists(oldSummingAndNode.second, fromNode))
                    {
                        auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                        graph_.disconnect(
                            oldSummingAndNode.second->outEdges().front()
                        );
                        graph_.connect(
                            newSummingAndNode.second,
                            channelPolarityInverters_[
                                ChannelListType::AudioHardwareOutputList
                            ][it->index].second,
                            0, 0
                        );
                        auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                            fromNode, newSummingAndNode.second,
                            0, newSummingAndNode.first->audioInputGroupCount() - 1
                        );
                        auto sendPosIt = sendPositions_.emplace(
                            sendIDGen_(), SendPosition {
                                .channelListType = type,
                                .channelIndex = channelIndex,
                                .sendIndex = sendPosition
                            }
                        ).first;
                        channelSendIDs_[type][channelIndex].emplace(
                            channelSendIDs_[type][channelIndex].begin() + sendPosition,
                            sendPosIt
                        );
                        FOR_RANGE(i, sendPosition + 1, channelSendIDs_[type][channelIndex].size())
                        {
                            ++(channelSendIDs_[type][channelIndex][i]->second.sendIndex);
                        }
                        channelSendPolarityInverters_[type][channelIndex].emplace(
                            channelSendPolarityInverters_[type][channelIndex].begin() + sendPosition,
                            std::move(polarityInverterAndNode)
                        );
                        channelSendMutes_[type][channelIndex].emplace(
                            channelSendMutes_[type][channelIndex].begin() + sendPosition,
                            std::move(muteAndNode)
                        );
                        channelSendFaders_[type][channelIndex].emplace(
                            channelSendFaders_[type][channelIndex].begin() + sendPosition,
                            std::move(faderAndNode)
                        );
                        channelSendDestinations_[type][channelIndex].emplace(
                            channelSendDestinations_[type][channelIndex].begin() + sendPosition,
                            destination
                        );
                        if(batchUpdater_)
                        {
                            if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second))
                            {
                                batchUpdater_->addObject(std::move(multiInputWithPDC));
                            }
                            batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                            oldSummingAndNode.first = std::move(newSummingAndNode.first);
                            std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                        }
                        else
                        {
                            auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingAndNode.second);
                            std::swap(newSummingAndNode.first, oldSummingAndNode.first);
                            std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                            connectionUpdatedCallback_(*this);
                        }
                        return true;
                    }
                    return false;
                }
            }
        }
        else if(destination.type == Position::Type::SendAndFXChannel)
        {
            if(auto& dests = channelSendDestinations_[type][channelIndex];
                std::ranges::find(dests, destination) == dests.end())
            {
                auto it = std::lower_bound(
                    idAndIndex.begin(), idAndIndex.end(),
                    destination.id
                );
                if(it != channelIdAndIndex_.end() && it->id == destination.id)
                {
                    auto channelType = channelInfos_[ChannelListType::RegularList][it->index].channelType;
                    if(channelType == ChannelType::AudioBus || channelType == ChannelType::AudioFX)
                    {
                        auto& oldSummingAndNode = inputDevices_[it->index];
                        auto fromNode = isPreFader? channelMutes_[type][channelIndex].second: channelFaders_[type][channelIndex].second;
                        if(!YADAW::Util::pathExists(oldSummingAndNode.second, fromNode))
                        {
                            auto newSummingAndNode = appendInputGroup(inputDevices_[it->index]);
                            graph_.disconnect(oldSummingAndNode.second->outEdges().front());
                            graph_.connect(
                                newSummingAndNode.second,
                                channelPolarityInverters_[ChannelListType::RegularList][it->index].second,
                                0, 0
                            );
                            auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                                fromNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1
                            );
                            auto sendPosIt = sendPositions_.emplace(
                                sendIDGen_(), SendPosition {
                                    .channelListType = type,
                                    .channelIndex = channelIndex,
                                    .sendIndex = sendPosition
                                }
                            ).first;
                            channelSendIDs_[type][channelIndex].emplace(
                                channelSendIDs_[type][channelIndex].begin() + sendPosition,
                                sendPosIt
                            );
                            FOR_RANGE(i, sendPosition + 1, channelSendIDs_[type][channelIndex].size())
                            {
                                ++(channelSendIDs_[type][channelIndex][i]->second.sendIndex);
                            }
                            channelSendPolarityInverters_[type][channelIndex].emplace(
                                channelSendPolarityInverters_[type][channelIndex].begin() + sendPosition,
                                std::move(polarityInverterAndNode)
                            );
                            channelSendMutes_[type][channelIndex].emplace(
                                channelSendMutes_[type][channelIndex].begin() + sendPosition,
                                std::move(muteAndNode)
                            );
                            channelSendFaders_[type][channelIndex].emplace(
                                channelSendFaders_[type][channelIndex].begin() + sendPosition,
                                std::move(faderAndNode)
                            );
                            channelSendDestinations_[type][channelIndex].emplace(
                                channelSendDestinations_[type][channelIndex].begin() + sendPosition,
                                destination
                            );
                            if(batchUpdater_)
                            {
                                if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second))
                                {
                                    batchUpdater_->addObject(std::move(multiInputWithPDC));
                                }
                                batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                                oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                                std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                            }
                            else
                            {
                                auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingAndNode.second);
                                std::unique_ptr<YADAW::Audio::Util::Summing> oldSumming(
                                    static_cast<YADAW::Audio::Util::Summing*>(oldSummingAndNode.first.release())
                                );
                                oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                                std::swap(newSummingAndNode.second, oldSummingAndNode.second);
                                connectionUpdatedCallback_(*this);
                            }
                            return true;
                        }
                        return false;
                    }
                    else if(channelType == ChannelType::Audio)
                    {
                        // not implemented
                        return false;
                    }
                }
            }
        }
        else if(destination.type == Position::Type::AudioChannel)
        {
            auto it = std::lower_bound(
                idAndIndex.begin(), idAndIndex.end(),
                destination.id
            );
            if(it != channelIdAndIndex_.end() && it->id == destination.id)
            {
                auto channelType = channelInfos_[ChannelListType::RegularList][it->index].channelType;
                if(channelType == ChannelType::Audio)
                {
                    if(mainInput_[it->index].type == Position::Type::Invalid)
                    {
                        auto& [toDevice, toNode] = inputDevices_[it->index];
                        auto fromNode = isPreFader? channelMutes_[type][channelIndex].second: channelFaders_[type][channelIndex].second;
                        if(channelGroupTypeAndChannelCountAt(type, channelIndex).value() == channelGroupTypeAndChannelCountAt(ChannelListType::RegularList, it->index)
                            && !YADAW::Util::pathExists(toNode, fromNode))
                        {
                            auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                                fromNode, toNode, 0, 1
                            );
                            auto sendPosIt = sendPositions_.emplace(
                                sendIDGen_(), SendPosition {
                                    .channelListType = type,
                                    .channelIndex = channelIndex,
                                    .sendIndex = sendPosition
                                }
                            ).first;
                            channelSendIDs_[type][channelIndex].emplace(
                                channelSendIDs_[type][channelIndex].begin() + sendPosition,
                                sendPosIt
                            );
                            mainInput_[it->index].type = Position::Type::Send;
                            mainInput_[it->index].id = sendPosIt->first;
                            channelSendPolarityInverters_[type][channelIndex].emplace(
                                channelSendPolarityInverters_[type][channelIndex].begin() + sendPosition,
                                std::move(polarityInverterAndNode)
                            );
                            channelSendMutes_[type][channelIndex].emplace(
                                channelSendMutes_[type][channelIndex].begin() + sendPosition,
                                std::move(muteAndNode)
                            );
                            channelSendFaders_[type][channelIndex].emplace(
                                channelSendFaders_[type][channelIndex].begin() + sendPosition,
                                std::move(faderAndNode)
                            );
                            channelSendDestinations_[type][channelIndex].emplace(
                                channelSendDestinations_[type][channelIndex].begin() + sendPosition,
                                destination
                            );
                            if(batchUpdater_)
                            {
                                batchUpdater_->addNull();
                            }
                            else
                            {
                                connectionUpdatedCallback_(*this);
                            }
                            return true;
                        }
                    }
                }
            }
            return false;
        }
        else if(destination.type == Position::Type::PluginAuxIO)
        {
            if(auto it = pluginAuxInputIDs_.find(destination.id);
                it != pluginAuxInputIDs_.end())
            {
                const auto& pluginAuxIOPosition = it->second;
                auto toNode = getNodeFromPluginAuxPosition(pluginAuxIOPosition);
                auto fromNode = isPreFader? channelMutes_[type][channelIndex].second: channelFaders_[type][channelIndex].second;
                const auto& fromChannelGroup = graph_.getNodeData(
                    fromNode
                ).process.device()->audioOutputGroupAt(0)->get();
                const auto& toChannelGroup = graph_.getNodeData(
                    toNode
                ).process.device()->audioInputGroupAt(
                    pluginAuxIOPosition.channelGroupIndex
                )->get();
                if(fromChannelGroup.type() == toChannelGroup.type()
                    && fromChannelGroup.channelCount() == toChannelGroup.channelCount()
                    && !YADAW::Util::pathExists(toNode, fromNode)
                )
                {
                    auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                        fromNode, toNode, 0, pluginAuxIOPosition.channelGroupIndex
                    );
                    auto sendPosIt = sendPositions_.emplace(
                        sendIDGen_(), SendPosition {
                            .channelListType = type,
                            .channelIndex = channelIndex,
                            .sendIndex = sendPosition
                        }
                    ).first;
                    channelSendIDs_[type][channelIndex].emplace(
                        channelSendIDs_[type][channelIndex].begin() + sendPosition,
                        sendPosIt
                    );
                    FOR_RANGE(i, sendPosition + 1, channelSendIDs_[type][channelIndex].size())
                    {
                        ++(channelSendIDs_[type][channelIndex][i]->second.sendIndex);
                    }
                    getAuxInputSource(pluginAuxIOPosition) = Position {
                        .type = Position::Type::Send,
                        .id = sendPosIt->first
                    };
                    channelSendPolarityInverters_[type][channelIndex].emplace(
                        channelSendPolarityInverters_[type][channelIndex].begin() + sendPosition,
                        std::move(polarityInverterAndNode)
                    );
                    channelSendMutes_[type][channelIndex].emplace(
                        channelSendMutes_[type][channelIndex].begin() + sendPosition,
                        std::move(muteAndNode)
                    );
                    channelSendFaders_[type][channelIndex].emplace(
                        channelSendFaders_[type][channelIndex].begin() + sendPosition,
                        std::move(faderAndNode)
                    );
                    channelSendDestinations_[type][channelIndex].emplace(
                        channelSendDestinations_[type][channelIndex].begin() + sendPosition,
                        destination
                    );
                    if(batchUpdater_)
                    {
                        batchUpdater_->addNull();
                    }
                    else
                    {
                        connectionUpdatedCallback_(*this);
                    }
                    return true;
                }
            }
            return false;
        }
    }
    return std::nullopt;
}

std::optional<bool> Mixer::setSendPreFader(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex,
    bool preFader)
{
    if(channelIndex < count(type) && sendIndex < sendCount(type, channelIndex))
    {
        if(sendIsPreFader(type, channelIndex, sendIndex) != preFader)
        {
            auto newInNode = preFader? channelMutes_[type][channelIndex].second:
                channelFaders_[type][channelIndex].second;
            auto polarityInverterNode = channelSendPolarityInverters_[
                type][channelIndex][sendIndex].second;
            graph_.disconnect(
                polarityInverterNode->inEdges().front()
            );
            graph_.connect(newInNode, polarityInverterNode, 0, 0);
            return true;
        }
        return false;
    }
    return std::nullopt;
}

std::optional<bool> Mixer::setSendDestination(
    ChannelListType type, std::uint32_t channelIndex, std::uint32_t sendIndex,
    Position destination)
{
    if(channelIndex < count(type) && sendIndex < sendCount(type, channelIndex))
    {
        auto& dests = channelSendDestinations_[type][channelIndex];
        auto oldDestination = dests[sendIndex];
        if(oldDestination == destination)
        {
            return true;
        }
        auto ret = false;
        if(std::ranges::find(dests, destination) == dests.end())
        {
            ade::NodeHandle inNode;
            std::uint32_t prevChannelIndex = 0;
            std::unique_ptr<YADAW::Audio::Device::IAudioDevice> oldSumming;
            ade::NodeHandle oldSummingNode;
            if(oldDestination.type == Position::Type::AudioHardwareIOChannel)
            {
                auto it = std::lower_bound(
                    channelIDAndIndex_[ChannelListType::AudioHardwareOutputList].begin(),
                    channelIDAndIndex_[ChannelListType::AudioHardwareOutputList].end(),
                    oldDestination.id
                );
                oldSummingNode = audioOutputSummings_[it->index].second;
            }
            else if(oldDestination.type == Position::Type::SendAndFXChannel)
            {
                auto it = std::lower_bound(
                    channelIDAndIndex_[ChannelListType::RegularList].begin(),
                    channelIDAndIndex_[ChannelListType::RegularList].end(),
                    oldDestination.id
                );
                oldSummingNode = inputDevices_[it->index].second;
            }
            else if(oldDestination.type == Position::Type::AudioChannel)
            {
                auto it = std::lower_bound(
                    channelIDAndIndex_[ChannelListType::RegularList].begin(),
                    channelIDAndIndex_[ChannelListType::RegularList].end(),
                    oldDestination.id
                );
                auto faderNode = channelSendFaders_[type][channelIndex][sendIndex].second;
                graph_.disconnect(faderNode->outEdges().front());
                mainInput_[it->index] = Position {};
            }
            else if(oldDestination.type == Position::Type::PluginAuxIO)
            {
                auto it = pluginAuxInputIDs_.find(oldDestination.id);
                getAuxInputSource(it->second) = Position {
                    .type = Position::Type::Invalid,
                    .id = IDGen::InvalidId
                };
            }
            if(oldSummingNode != nullptr)
            {
                inNode = channelSendPolarityInverters_[type][channelIndex][sendIndex].second->inNodes().front();
                for(const auto& outEdge: inNode->outEdges())
                {
                    if(outEdge->dstNode() == oldSummingNode)
                    {
                        prevChannelIndex = graph_.getEdgeData(outEdge).toChannel;
                        break;
                    }
                }
            }
            if(destination.type == Position::Type::AudioHardwareIOChannel)
            {
                auto it = std::lower_bound(
                    channelIDAndIndex_[ChannelListType::AudioHardwareOutputList].begin(),
                    channelIDAndIndex_[ChannelListType::AudioHardwareOutputList].end(),
                    destination.id
                );
                if(it != channelIDAndIndex_[ChannelListType::AudioHardwareOutputList].end()
                    && it->id == destination.id)
                {
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    if(!YADAW::Util::pathExists(oldSummingAndNode.second, inNode))
                    {
                        auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                        graph_.connect(inNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1);
                        oldSumming = std::move(oldSummingAndNode.first);
                        oldSummingNode = oldSummingAndNode.second;
                        oldSummingAndNode = std::move(newSummingAndNode);
                        ret = true;
                    }
                }
            }
            else if(destination.type == Position::Type::SendAndFXChannel)
            {
                auto it = std::lower_bound(
                    channelIDAndIndex_[ChannelListType::RegularList].begin(),
                    channelIDAndIndex_[ChannelListType::RegularList].end(),
                    destination.id
                );
                if(it != channelIDAndIndex_[ChannelListType::RegularList].end()
                    && it->id == destination.id)
                {
                    auto type = channelInfo_[it->index].channelType;
                    if(type == ChannelType::AudioBus || type == ChannelType::AudioFX)
                    {
                        auto& oldSummingAndNode = inputDevices_[it->index];
                        if(!YADAW::Util::pathExists(oldSummingAndNode.second, inNode))
                        {
                            auto newSummingAndNode = appendInputGroup(oldSummingAndNode);
                            graph_.connect(inNode, newSummingAndNode.second, 0, newSummingAndNode.first->audioInputGroupCount() - 1);
                            oldSumming.reset(oldSummingAndNode.first.release());
                            oldSummingNode = oldSummingAndNode.second;
                            oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                            oldSummingAndNode.second = std::move(newSummingAndNode.second);
                            ret = true;
                        }
                    }
                }
            }
            else if(destination.type == Position::Type::AudioChannel)
            {
                auto it = std::lower_bound(
                    channelIDAndIndex_[ChannelListType::RegularList].begin(),
                    channelIDAndIndex_[ChannelListType::RegularList].end(),
                    destination.id
                );
                if(it != channelIDAndIndex_[ChannelListType::RegularList].end()
                    && it->id == destination.id)
                {
                    if(channelInfo_[it->index].channelType == ChannelType::Audio
                        && mainInput_[it->index].type == Position::Type::Invalid
                        && channelGroupTypeAndChannelCountAt(type, channelIndex) == channelGroupTypeAndChannelCountAt(ChannelListType::RegularList, it->index))
                    {
                        graph_.connect(inNode, inputDevices_[it->index].second, 0, 1);
                        ret = true;
                    }
                }
            }
            else if(destination.type == Position::Type::PluginAuxIO)
            {
                if(auto it = pluginAuxInputIDs_.find(destination.id);
                    it != pluginAuxInputIDs_.end())
                {
                    const auto& pluginAuxIOPosition = it->second;
                    auto toNode = getNodeFromPluginAuxPosition(pluginAuxIOPosition);
                    auto fromNode = channelSendPolarityInverters_[type][channelIndex][sendIndex].second->inNodes().front();
                    const auto& fromChannelGroup = channelSendFaders_[type][channelIndex][sendIndex].first->audioOutputGroupAt(0)->get();
                    const auto& toChannelGroup = graph_.getNodeData(
                        toNode
                    ).process.device()->audioInputGroupAt(
                        pluginAuxIOPosition.channelGroupIndex
                    )->get();
                    if(fromChannelGroup.type() == toChannelGroup.type()
                        && fromChannelGroup.channelCount() == toChannelGroup.channelCount()
                        && !YADAW::Util::pathExists(toNode, fromNode)
                    )
                    {
                        auto [polarityInverterAndNode, muteAndNode, faderAndNode] = createSend(
                            fromNode, toNode, 0, pluginAuxIOPosition.channelGroupIndex
                        );
                        auto sendPosIt = sendPositions_.emplace(
                            sendIDGen_(), SendPosition {
                                .channelListType = type,
                                .channelIndex = channelIndex,
                                .sendIndex = sendIndex
                            }
                        ).first;
                        getAuxInputSource(pluginAuxIOPosition) = Position {
                            .type = Position::Type::Send,
                            .id = sendPosIt->first
                        };
                        if(batchUpdater_)
                        {
                            batchUpdater_->addNull();
                        }
                        else
                        {
                            connectionUpdatedCallback_(*this);
                        }
                        ret = true;
                    }
                }
                return false;
            }
            if(ret)
            {
                dests[sendIndex] = destination;
                if(batchUpdater_)
                {
                    if(auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingNode))
                    {
                        batchUpdater_->addObject(std::move(multiInputWithPDC));
                    }
                }
                else
                {
                    auto disposingOldSumming = graphWithPDC_.removeNode(oldSummingNode);
                    connectionUpdatedCallback_(*this);
                }
            }
            else if(oldSummingNode != nullptr)
            {
                graph_.connect(inNode, oldSummingNode, 0, prevChannelIndex);
            }
            return ret;
        }
    }
    return std::nullopt;
}

std::optional<bool> Mixer::removeSend(
    ChannelListType type, std::uint32_t channelIndex,
    std::uint32_t sendPosition, std::uint32_t removeCount)
{
    if(channelIndex < count(type))
    {
        auto& sendIDs = channelSendIDs_[type][channelIndex];
        auto& sendDestinations = channelSendDestinations_[type][channelIndex];
        auto& sendFaders = channelSendFaders_[type][channelIndex];
        auto& sendPolarityInverters = channelSendPolarityInverters_[type][channelIndex];
        auto& sendMutes = channelSendMutes_[type][channelIndex];
        auto sendCount = sendDestinations.size();
        if(sendPosition < sendCount && sendPosition + removeCount <= sendCount)
        {
            std::vector<Position> destinations; destinations.reserve(removeCount);
            FOR_RANGE(i, sendPosition, sendPosition + removeCount)
            {
                destinations.emplace_back(sendDestinations[i]); // FIXME
                graph_.removeNode(sendFaders[i].second);
                graph_.removeNode(sendMutes[i].second);
                graph_.removeNode(sendPolarityInverters[i].second);
            }
            std::vector<SummingAndNode> removingSummings;
            removingSummings.reserve(destinations.size());
            FOR_RANGE0(i, destinations.size())
            {
                auto destination = destinations[i];
                if(destination.type == Position::Type::AudioHardwareIOChannel)
                {
                    auto it = std::lower_bound(
                        audioOutputChannelIdAndIndex_.begin(),
                        audioOutputChannelIdAndIndex_.end(),
                        destination.id
                    );
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    removingSummings.emplace_back(
                        shrinkInputGroups(oldSummingAndNode)
                    ).swap(oldSummingAndNode);
                }
                else if(destination.type == Position::Type::SendAndFXChannel)
                {
                    auto it = std::lower_bound(
                        channelIdAndIndex_.begin(),
                        channelIdAndIndex_.end(),
                        destination.id
                    );
                    auto& oldSummingAndNode = inputDevices_[it->index];
                    auto& newSummingAndNode = removingSummings.emplace_back(shrinkInputGroups(oldSummingAndNode));
                    std::swap(oldSummingAndNode.second, newSummingAndNode.second);
                    auto ptr = oldSummingAndNode.first.release();
                    oldSummingAndNode.first.reset(newSummingAndNode.first.release());
                }
                else if(destination.type == Position::Type::AudioChannel)
                {
                    auto it = std::lower_bound(
                        channelIDAndIndex_[ChannelListType::RegularList].begin(),
                        channelIDAndIndex_[ChannelListType::RegularList].end(),
                        destination.id
                    );
                    mainInput_[it->index] = Position {};
                }
                else if(destination.type == Position::Type::PluginAuxIO)
                {
                    auto it = pluginAuxInputIDs_.find(destination.id);
                    getAuxInputSource(it->second) = Position {};
                }
            }
            if(batchUpdater_)
            {
                FOR_RANGE(i, sendPosition, sendPosition + removeCount)
                {
                    batchUpdater_->addObject(std::move(sendFaders[i]).first);
                    batchUpdater_->addObject(std::move(sendMutes[i]).first);
                    batchUpdater_->addObject(std::move(sendPolarityInverters[i]).first);
                }
            }
            else
            {
                connectionUpdatedCallback_(*this);
            }
            FOR_RANGE(i, sendPosition, sendPosition + removeCount)
            {
                sendPositions_.erase(sendIDs[i]);
            }
            sendIDs.erase(
                sendIDs.begin() + sendPosition,
                sendIDs.begin() + sendPosition + removeCount
            );
            FOR_RANGE(i, sendPosition, sendIDs.size())
            {
                sendIDs[i]->second.sendIndex -= removeCount;
            }
            sendDestinations.erase(
                sendDestinations.begin() + sendPosition,
                sendDestinations.begin() + sendPosition + removeCount
            );
            sendFaders.erase(
                sendFaders.begin() + sendPosition,
                sendFaders.begin() + sendPosition + removeCount
            );
            sendMutes.erase(
                sendMutes.begin() + sendPosition,
                sendMutes.begin() + sendPosition + removeCount
            );
            sendPolarityInverters.erase(
                sendPolarityInverters.begin() + sendPosition,
                sendPolarityInverters.begin() + sendPosition + removeCount
            );
            return true;
        }
        return false;
    }
    return std::nullopt;
}
std::optional<bool> Mixer::clearSends(
    ChannelListType type, std::uint32_t channelIndex)
{
    if(channelIndex < count(type))
    {
        return removeSend(type, channelIndex, 0, *sendCount(type, channelIndex));
    }
    return std::nullopt;
}

OptionalRef<const Mixer::Position> Mixer::mainInputAt(std::uint32_t index) const
{
    if(index < count(ChannelListType::RegularList)
        && channelInfo_[index].channelType == ChannelType::Audio)
    {
        return {mainInput_[index]};
    }
    return std::nullopt;
}

bool Mixer::setMainInputAt(std::uint32_t index, Position position)
{
    if(index < count(ChannelListType::RegularList)
        && channelInfo_[index].channelType == ChannelType::Audio)
    {
        // Disconnect
        if(auto oldPosition = mainInput_[index]; oldPosition.type == Position::Type::Send)
        {
            auto&& sendPosition = getSendPosition(oldPosition.id).value();
            removeSend(sendPosition.channelListType, sendPosition.channelIndex, sendPosition.sendIndex);
        }
        else if(oldPosition.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
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
        else if(position.type == Position::Type::SendAndFXChannel)
        {
            // not implemented
        }
        else if(position.type == Position::Type::PluginAuxIO)
        {
            // not implemented
        }
        if(ret)
        {
            if(batchUpdater_)
            {
                batchUpdater_->addNull();
            }
            else
            {
                connectionUpdatedCallback_(*this);
            }
            mainInput_[index] = position;
        }
        return ret;
    }
    return false;
}

OptionalRef<const Mixer::Position> Mixer::mainOutputAt(std::uint32_t index) const
{
    if(index < count(ChannelListType::RegularList))
    {
        return {mainOutput_[index]};
    }
    return std::nullopt;
}

bool Mixer::setMainOutputAt(std::uint32_t index, Position position)
{
    if(index >= count(ChannelListType::RegularList))
    {
        return false;
    }
    auto ret = false;
    const auto& mute = *(mutes_[index].first);
    const auto& channelGroup = mute.audioOutputGroupAt(0)->get();
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
                auto& oldSummingAndNode = audioOutputSummings_[it->index];
                auto& [oldSumming, oldSummingNode] = oldSummingAndNode;
                auto inEdges = oldSummingNode->inEdges();
                auto toChannel = graph_.getEdgeData(
                    *std::find_if(
                        inEdges.begin(), inEdges.end(),
                        [&fromNode](const ade::EdgeHandle& inEdge)
                        {
                            return inEdge->srcNode() == fromNode;
                        }
                    )
                ).toChannel;
                auto [newSumming, newSummingNode] = removeInputGroup(oldSummingAndNode, toChannel).value();
                graph_.disconnect(
                        oldSummingNode->outEdges().front()
                );
                graph_.connect(
                    newSummingNode,
                    audioOutputPolarityInverters_[it->index].second,
                    0, 0
                );
                disconnectingOldMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                disconnectingOldSumming = std::move(oldSumming);
                oldSumming = std::move(newSumming);
                oldSummingNode = newSummingNode;
            }
        }
        else if(oldPosition.type == Position::Type::SendAndFXChannel)
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
                    auto& oldSummingAndNode = inputDevices_[it->index];
                    auto& [oldSumming, oldSummingNode] = oldSummingAndNode;
                    auto inEdges = oldSummingNode->inEdges();
                    auto toChannel = graph_.getEdgeData(
                        *std::find_if(
                            inEdges.begin(), inEdges.end(),
                            [&fromNode](const ade::EdgeHandle& inEdge)
                            {
                                return inEdge->srcNode() == fromNode;
                            }
                        )
                    ).toChannel;
                    auto [newSumming, newSummingNode] = removeInputGroup(oldSummingAndNode, toChannel).value();
                    graph_.disconnect(
                        oldSummingNode->outEdges().front()
                    );
                    graph_.connect(
                        newSummingNode,
                        polarityInverters_[it->index].second,
                        0, 0
                    );
                    disconnectingOldMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                    disconnectingOldSumming = std::move(oldSumming);
                    oldSumming = std::move(newSumming);
                    oldSummingNode = newSummingNode;
                }
            }
        }
        else if(oldPosition.type == Position::Type::PluginAuxIO)
        {
            auto it = pluginAuxInputIDs_.find(oldPosition.id);
            const auto& pluginAuxIOPosition = it->second;
            auto node = getNodeFromPluginAuxPosition(pluginAuxIOPosition);
            for(const auto& edgeHandle: node->inEdges())
            {
                if(graph_.getEdgeData(edgeHandle).toChannel == pluginAuxIOPosition.channelGroupIndex)
                {
                    graph_.disconnect(edgeHandle);
                    getAuxInputSource(pluginAuxIOPosition) = Position {
                        .type = Position::Type::Invalid,
                        .id = IDGen::InvalidId
                    };
                    if(batchUpdater_)
                    {
                        batchUpdater_->addNull();
                    }
                    else
                    {
                        connectionUpdatedCallback_(*this);
                    }
                    // TODO
                    break;
                }
            }
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
                const auto& destPair = *channelGroupTypeAndChannelCountAt(
                    ChannelListType::AudioHardwareOutputList,
                    outputChannelIndex
                );
                const auto& srcPair = *channelGroupTypeAndChannelCountAt(
                    ChannelListType::RegularList, index
                );
                if(srcPair == destPair)
                {
                    auto fromNode = postFaderInserts_[index]->outNode();
                    auto& oldSummingAndNode = audioOutputSummings_[outputChannelIndex];
                    auto& [oldSumming, oldSummingNode] = oldSummingAndNode;
                    auto [newSumming, newSummingNode] = appendInputGroup(
                        oldSummingAndNode
                    );
                    graph_.connect(
                        fromNode, newSummingNode, 0, newSumming->audioInputGroupCount() - 1
                    );
                    graph_.disconnect(audioOutputPolarityInverters_[it->index].second->inEdges().front());
                    graph_.connect(newSummingNode, audioOutputPolarityInverters_[it->index].second, 0, 0);
                    disconnectingNewMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                    disconnectingNewSumming = std::move(oldSumming);
                    if(batchUpdater_)
                    {
                        batchUpdater_->addObject(
                            std::move(disconnectingOldSumming)
                        );
                    }
                    else
                    {
                        connectionUpdatedCallback_(*this);
                        disconnectingOldSumming.reset();
                        disconnectingOldMultiInput.reset();
                        disconnectingNewSumming.reset();
                        disconnectingNewMultiInput.reset();
                    }
                    oldSumming = std::move(newSumming);
                    oldSummingNode = newSummingNode;
                    ret = true;
                }
            }
        }
        else if(position.type == Position::Type::SendAndFXChannel)
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
                        *channelGroupTypeAndChannelCountAt(ChannelListType::RegularList, outputChannelIndex);
                    const auto& srcPair =
                        *channelGroupTypeAndChannelCountAt(ChannelListType::RegularList, index);
                    if(srcPair == destPair)
                    {
                        auto fromNode = postFaderInserts_[index]->outNode();
                        auto& oldSummingAndNode = inputDevices_[outputChannelIndex];
                        auto& [oldSumming, oldSummingNode] = oldSummingAndNode;
                        auto [newSumming, newSummingNode] = appendInputGroup(
                            oldSummingAndNode
                        );
                        graph_.connect(
                            fromNode, newSummingNode, 0, newSumming->audioInputGroupCount() - 1
                        );
                        graph_.disconnect(polarityInverters_[it->index].second->inEdges().front());
                        graph_.connect(newSummingNode, polarityInverters_[it->index].second, 0, 0);
                        disconnectingNewSumming = std::move(oldSumming);
                        if(batchUpdater_)
                        {
                            batchUpdater_->addObject(std::move(disconnectingOldSumming));
                            batchUpdater_->addObject(std::move(disconnectingNewSumming));
                            batchUpdater_->addObject(std::move(disconnectingOldMultiInput));
                            batchUpdater_->addObject(std::move(disconnectingNewMultiInput));
                        }
                        else
                        {
                            disconnectingNewMultiInput = graphWithPDC_.removeNode(oldSummingNode);
                            connectionUpdatedCallback_(*this);
                            disconnectingOldSumming.reset();
                            disconnectingNewSumming.reset();
                            disconnectingOldMultiInput.reset();
                            disconnectingNewMultiInput.reset();
                        }
                        oldSumming = std::move(newSumming);
                        oldSummingNode = newSummingNode;
                        ret = true;
                    }
                }
            }
        }
        else if(position.type == Position::Type::PluginAuxIO)
        {
            if(auto it = pluginAuxInputIDs_.find(position.id); it != pluginAuxInputIDs_.end())
            {
                const auto& pluginAuxIOPosition = it->second;
                auto node = getNodeFromPluginAuxPosition(pluginAuxIOPosition);
                auto device = graph_.getNodeData(node).process.device();
                const auto& destChannelGroup = device->audioInputGroupAt(pluginAuxIOPosition.channelGroupIndex)->get();
                auto inEdges = node->inEdges();
                auto fromNode = postFaderInserts_[index]->outNode();
                if(
                    destChannelGroup.type() == channelGroup.type()
                    && destChannelGroup.channelCount() == channelGroup.channelCount()
                    && std::none_of(
                        inEdges.begin(), inEdges.end(),
                        [this, channelGroupIndex = pluginAuxIOPosition.channelGroupIndex](const ade::EdgeHandle& edge)
                        {
                            return graph_.getEdgeData(edge).toChannel == channelGroupIndex;
                        }
                    )
                    && !YADAW::Util::pathExists(node, fromNode)
                )
                {
                    graph_.connect(
                        fromNode, node, 0, pluginAuxIOPosition.channelGroupIndex
                    );
                    getAuxInputSource(pluginAuxIOPosition) = Position {
                        .type = Position::Type::SendAndFXChannel,
                        .id = channelId_[index]
                    };
                    if(batchUpdater_)
                    {
                        batchUpdater_->addNull();
                    }
                    else
                    {
                        connectionUpdatedCallback_(*this);
                    }
                    ret = true;
                }
            }
        }
        else if(position.type == Position::Type::Invalid)
        {
            if(batchUpdater_)
            {
                batchUpdater_->addObject(std::move(disconnectingOldMultiInput));
                batchUpdater_->addObject(std::move(disconnectingOldSumming));
            }
            else
            {
                connectionUpdatedCallback_(*this);
                disconnectingOldMultiInput.reset();
                disconnectingOldSumming.reset();
            }
            ret = true;
        }
    }
    if(ret)
    {
        oldPosition = position;
    }
    return ret;
}

bool Mixer::appendAudioInputChannel(
    const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex)
{
    return insertAudioInputChannel(count(ChannelListType::AudioHardwareInputList), inNode, channelGroupIndex);
}

bool Mixer::insertAudioInputChannel(std::uint32_t position,
    const ade::NodeHandle& inNode, std::uint32_t channelGroupIndex)
{
    auto device = graph_.getNodeData(inNode).process.device();
    if(channelGroupIndex < device->audioOutputGroupCount()
       && position <= count(ChannelListType::AudioHardwareInputList))
    {
        preInsertAudioInputChannelCallback_(
            *this,
            Mixer::PreInsertChannelCallbackArgs {.position = position, .count = 1U}
        );
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
                graph_, auxInputIdGen_, auxOutputIdGen_,
                polarityInverterNode, muteNode, channelGroupIndex, 0, batchUpdater_
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
                graph_, auxInputIdGen_, auxOutputIdGen_,
                faderNode, meterNode, 0, 0, batchUpdater_
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
        if(batchUpdater_)
        {
            batchUpdater_->addNull();
        }
        else
        {
            connectionUpdatedCallback_(*this);
        }
        auto& info = *audioInputChannelInfo_.emplace(audioInputChannelInfo_.begin() + position);
        info.channelType = ChannelType::AudioBus;
        audioInputSendIDs_.emplace(audioInputSendIDs_.begin() + position);
        FOR_RANGE(i, position + 1, audioInputSendIDs_.size())
        {
            for(auto& it: audioInputSendIDs_[i])
            {
                ++(it->second.channelIndex);
            }
        }
        audioInputSendMutes_.emplace(audioInputSendMutes_.begin() + position);
        audioInputSendFaders_.emplace(audioInputSendFaders_.begin() + position);
        audioInputSendPolarityInverters_.emplace(audioInputSendPolarityInverters_.begin() + position);
        audioInputSendDestinations_.emplace(audioInputSendDestinations_.begin() + position);
        auto& vecInput = pluginAuxInputs_[ChannelListType::AudioHardwareInputList];
        using ChannelData = decltype(pluginAuxInputs_)::value_type::value_type;
        vecInput.emplace(vecInput.begin() + position, ChannelData(
                ChannelData::first_type(),
                ChannelData::second_type(2)
            )
        );
        auto& vecOutput = pluginAuxOutputs_[ChannelListType::AudioHardwareInputList];
        vecOutput.emplace(vecOutput.begin() + position, ChannelData(
                ChannelData::first_type(),
                ChannelData::second_type(2)
            )
        );
        auto& vecInputSources = pluginAuxInputSources_[ChannelListType::AudioHardwareInputList];
        vecInputSources.emplace(vecInputSources.begin() + position,
            PluginAuxInputSources::value_type::value_type(
                PluginAuxInputSources::value_type::value_type::first_type(),
                PluginAuxInputSources::value_type::value_type::second_type(2)
            )
        );
        auto& vecOutputDestinations = pluginAuxOutputDestinations_[ChannelListType::AudioHardwareInputList];
        vecOutputDestinations.emplace(vecOutputDestinations.begin() + position,
            PluginAuxOutputDestinations::value_type::value_type(
                PluginAuxOutputDestinations::value_type::value_type::first_type(),
                PluginAuxOutputDestinations::value_type::value_type::second_type(2)
            )
        );
        audioInputPreFaderInserts_[position]->setInsertCallbackUserData(
            YADAW::Util::createPMRUniquePtr(
                std::make_unique<InsertPosition>(
                    *this, ChannelListType::AudioHardwareInputList, position, 0U
                )
            )
        );
        audioInputPostFaderInserts_[position]->setInsertCallbackUserData(
            YADAW::Util::createPMRUniquePtr(
                std::make_unique<InsertPosition>(
                    *this, ChannelListType::AudioHardwareInputList, position, 1U
                )
            )
        );
        audioInputPreFaderInserts_[position]->setInsertAddedCallback(&Mixer::insertAdded);
        audioInputPreFaderInserts_[position]->setInsertRemovedCallback(&Mixer::insertRemoved);
        audioInputPostFaderInserts_[position]->setInsertAddedCallback(&Mixer::insertAdded);
        audioInputPostFaderInserts_[position]->setInsertRemovedCallback(&Mixer::insertRemoved);
        updatePluginAuxPosition(
            ChannelListType::AudioHardwareInputList, position + 1
        );
        return true;
    }
    return false;
}

bool Mixer::appendAudioOutputChannel(
    const ade::NodeHandle& outNode, std::uint32_t channelGroupIndex)
{
    return insertAudioOutputChannel(count(ChannelListType::AudioHardwareOutputList), outNode, channelGroupIndex);
}

bool Mixer::insertAudioOutputChannel(std::uint32_t position,
    const ade::NodeHandle& outNode, std::uint32_t channel)
{
    // input device of an audio output channel is a summing.
    auto device = graph_.getNodeData(outNode).process.device();
    if(channel < device->audioInputGroupCount()
        && position <= count(ChannelListType::AudioHardwareOutputList))
    {
        preInsertAudioOutputChannelCallback_(
            *this,
            Mixer::PreInsertChannelCallbackArgs {.position = position, .count = 1U}
        );
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
                graph_, auxInputIdGen_, auxOutputIdGen_,
                polarityInverterNode, muteNode, 0, 0, batchUpdater_
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
                graph_, auxInputIdGen_, auxOutputIdGen_,
                faderNode, meterNode, 0, 0, batchUpdater_
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
        if(batchUpdater_)
        {
            batchUpdater_->addNull();
        }
        else
        {
            connectionUpdatedCallback_(*this);
        }
        auto& info = *audioOutputChannelInfo_.emplace(audioOutputChannelInfo_.begin() + position);
        info.channelType = ChannelType::AudioBus;
        audioOutputSendIDs_.emplace(audioOutputSendIDs_.begin() + position);
        FOR_RANGE(i, position + 1, audioOutputSendIDs_.size())
        {
            for(const auto& it: audioInputSendIDs_[i])
            {
                ++(it->second.channelIndex);
            }
        }
        audioOutputSendMutes_.emplace(audioOutputSendMutes_.begin() + position);
        audioOutputSendFaders_.emplace(audioOutputSendFaders_.begin() + position);
        audioOutputSendPolarityInverters_.emplace(audioOutputSendPolarityInverters_.begin() + position);
        audioOutputSendDestinations_.emplace(audioOutputSendDestinations_.begin() + position);
        auto& vecInput = pluginAuxInputs_[ChannelListType::AudioHardwareOutputList];
        using ChannelData = decltype(pluginAuxInputs_)::value_type::value_type;
        vecInput.emplace(vecInput.begin() + position, ChannelData(
                ChannelData::first_type(),
                ChannelData::second_type(2)
            )
        );
        auto& vecOutput = pluginAuxOutputs_[ChannelListType::AudioHardwareOutputList];
        vecOutput.emplace(vecOutput.begin() + position, ChannelData(
                ChannelData::first_type(),
                ChannelData::second_type(2)
            )
        );
        auto& vecInputSources = pluginAuxInputSources_[ChannelListType::AudioHardwareOutputList];
        vecInputSources.emplace(vecInputSources.begin() + position,
            PluginAuxInputSources::value_type::value_type(
                PluginAuxInputSources::value_type::value_type::first_type(),
                PluginAuxInputSources::value_type::value_type::second_type(2)
            )
        );
        auto& vecOutputDestinations = pluginAuxOutputDestinations_[ChannelListType::AudioHardwareOutputList];
        vecOutputDestinations.emplace(vecOutputDestinations.begin() + position,
            PluginAuxOutputDestinations::value_type::value_type(
                PluginAuxOutputDestinations::value_type::value_type::first_type(),
                PluginAuxOutputDestinations::value_type::value_type::second_type(2)
            )
        );
        audioOutputPreFaderInserts_[position]->setInsertCallbackUserData(
            YADAW::Util::createPMRUniquePtr(
                std::make_unique<InsertPosition>(
                    *this, ChannelListType::AudioHardwareOutputList, position, 0U
                )
            )
        );
        audioOutputPostFaderInserts_[position]->setInsertCallbackUserData(
            YADAW::Util::createPMRUniquePtr(
                std::make_unique<InsertPosition>(
                    *this, ChannelListType::AudioHardwareOutputList, position, 1U
                )
            )
        );
        audioOutputPreFaderInserts_[position]->setInsertAddedCallback(&Mixer::insertAdded);
        audioOutputPreFaderInserts_[position]->setInsertRemovedCallback(&Mixer::insertRemoved);
        audioOutputPostFaderInserts_[position]->setInsertAddedCallback(&Mixer::insertAdded);
        audioOutputPostFaderInserts_[position]->setInsertRemovedCallback(&Mixer::insertRemoved);
        updatePluginAuxPosition(
            ChannelListType::AudioHardwareOutputList, position + 1
        );
        return true;
    }
    return false;
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
    if(position <= this->count(ChannelListType::RegularList) && count > 0)
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
                    graph_, auxInputIdGen_, auxOutputIdGen_,
                    fadersAndNode[i].second, metersAndNode[i].second, 0, 0, batchUpdater_
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
                auto node = graphWithPDC_.addNode(
                    YADAW::Audio::Engine::AudioDeviceProcess(
                        *inputSwitchers[i]
                    )
                );
                auto multiInput = static_cast<YADAW::Audio::Engine::MultiInputDeviceWithPDC*>(
                    graph_.getNodeData(node).process.device()
                );
                multiInput->setBufferSize(bufferExtension().bufferSize());
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
        preInsertRegularChannelCallback_(
            *this,
            Mixer::PreInsertChannelCallbackArgs {.position = position, .count = count}
        );
        // instruments -----------------------------------------------------
        std::fill_n(
            std::inserter(
                instrumentBypassed_,
                instrumentBypassed_.begin() + position
            ),
            count, false
        );
        std::fill_n(
            std::inserter(
                instrumentOutputChannelIndex_,
                instrumentOutputChannelIndex_.begin() + position
            ),
            count, 0U
        );
        std::generate_n(
            std::inserter(
                instrumentContexts_, instrumentContexts_.begin() + position
            ),
            count,
            []()
            {
                return YADAW::Util::createUniquePtr(nullptr);
            }
        );
        // pre-fader inserts -----------------------------------------------
        std::vector<std::unique_ptr<YADAW::Audio::Mixer::Inserts>> preFaderInserts;
        preFaderInserts.reserve(count);
        FOR_RANGE0(i, count)
        {
            preFaderInserts.emplace_back(
                std::make_unique<YADAW::Audio::Mixer::Inserts>(
                    graph_, auxInputIdGen_, auxOutputIdGen_,
                    polarityInvertersAndNode[i].second, mutesAndNode[i].second,
                    0, 0, batchUpdater_
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
        // send
        std::generate_n(
            std::inserter(
                sendIDs_,
                sendIDs_.begin() + position), count,
            []() -> std::decay_t<decltype(sendIDs_)>::value_type
            {
                return {};
            }
        );
        FOR_RANGE(i, position + 1, sendIDs_.size())
        {
            for(const auto& it: sendIDs_[i])
            {
                ++(it->second.channelIndex);
            }
        }
        std::generate_n(
            std::inserter(
                sendPolarityInverters_,
                sendPolarityInverters_.begin() + position), count,
            []() -> std::decay_t<decltype(sendPolarityInverters_)>::value_type
            {
                return {};
            }
        );
        std::generate_n(
            std::inserter(
                sendMutes_,
                sendMutes_.begin() + position), count,
            []() -> std::decay_t<decltype(sendMutes_)>::value_type
            {
                return {};
            }
        );
        std::generate_n(
            std::inserter(
                sendFaders_,
                sendFaders_.begin() + position), count,
            []() -> std::decay_t<decltype(sendFaders_)>::value_type
            {
                return {};
            }
        );
        std::fill_n(
            std::inserter(
                sendDestinations_,
                sendDestinations_.begin() + position), count,
                std::decay_t<decltype(sendDestinations_)>::value_type()
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
            Position {Position::Invalid, IDGen::InvalidId}
        );
        std::fill_n(
            std::inserter(mainOutput_, mainOutput_.begin() + position), count,
            Position {Position::Invalid, IDGen::InvalidId}
        );
        auto& vecInput = pluginAuxInputs_[ChannelListType::RegularList];
        using ChannelData = decltype(pluginAuxInputs_)::value_type::value_type;
        vecInput.insert(
            vecInput.begin() + position, count, ChannelData(
                ChannelData::first_type(),
                ChannelData::second_type(2)
            )
        );
        auto& vecOutput = pluginAuxOutputs_[ChannelListType::RegularList];
        vecOutput.insert(
            vecOutput.begin() + position, count, ChannelData(
                ChannelData::first_type(),
                ChannelData::second_type(2)
            )
        );
        auto& vecInputSources = pluginAuxInputSources_[ChannelListType::RegularList];
        vecInputSources.insert(vecInputSources.begin() + position, count,
            PluginAuxInputSources::value_type::value_type(
                PluginAuxInputSources::value_type::value_type::first_type(),
                PluginAuxInputSources::value_type::value_type::second_type(2)
            )
        );
        auto& vecOutputDestinations = pluginAuxOutputDestinations_[ChannelListType::RegularList];
        vecOutputDestinations.insert(vecOutputDestinations.begin() + position, count,
            PluginAuxOutputDestinations::value_type::value_type(
                PluginAuxOutputDestinations::value_type::value_type::first_type(),
                PluginAuxOutputDestinations::value_type::value_type::second_type(2)
            )
        );
        FOR_RANGE(i, position, position + count)
        {
            preFaderInserts_[i]->setInsertCallbackUserData(
                YADAW::Util::createPMRUniquePtr(
                    std::make_unique<InsertPosition>(
                        *this, ChannelListType::RegularList, i, 0U
                    )
                )
            );
            postFaderInserts_[i]->setInsertCallbackUserData(
                YADAW::Util::createPMRUniquePtr(
                    std::make_unique<InsertPosition>(
                        *this, ChannelListType::RegularList, i, 1U
                    )
                )
            );
            preFaderInserts_[i]->setInsertAddedCallback(&Mixer::insertAdded);
            preFaderInserts_[i]->setInsertRemovedCallback(&Mixer::insertRemoved);
            postFaderInserts_[i]->setInsertAddedCallback(&Mixer::insertAdded);
            postFaderInserts_[i]->setInsertRemovedCallback(&Mixer::insertRemoved);
        }
        updatePluginAuxPosition(
            ChannelListType::RegularList, position + count
        );
        if(batchUpdater_)
        {
            batchUpdater_->addNull();
        }
        else
        {
            connectionUpdatedCallback_(*this);
        }
    }
    return ret;
}

bool Mixer::appendChannels(
    std::uint32_t count, ChannelType channelType,
    YADAW::Audio::Base::ChannelGroupType channelGroupType,
    std::uint32_t channelCountInGroup)
{
    return insertChannels(
        this->count(ChannelListType::RegularList), count, channelType, channelGroupType, channelCountInGroup);
}

ade::NodeHandle Mixer::getInstrument(std::uint32_t index) const
{
    if(index < count(ChannelListType::RegularList)
        && channelInfo_[index].channelType == ChannelType::Instrument)
    {
        return inputDevices_[index].second;
    }
    return {};
}

OptionalRef<const Context> Mixer::getInstrumentContext(std::uint32_t index) const
{
    if(index < count(ChannelListType::RegularList)
        && channelInfo_[index].channelType == ChannelType::Instrument
        && instrumentContexts_[index])
    {
        return instrumentContexts_[index];
    }
    return {};
}

OptionalRef<Context> Mixer::getInstrumentContext(std::uint32_t index)
{
    if(index < count(ChannelListType::RegularList)
        && channelInfo_[index].channelType == ChannelType::Instrument
        && instrumentContexts_[index])
    {
        return instrumentContexts_[index];
    }
    return {};
}

std::optional<std::uint32_t> Mixer::getInstrumentMainOutputChannelGroupIndex(std::uint32_t index) const
{
    if(index < count(ChannelListType::RegularList)
        && channelInfo_[index].channelType == ChannelType::Instrument
        && instrumentContexts_[index])
    {
        return instrumentOutputChannelIndex_[index];
    }
    return {};
}

bool Mixer::setInstrument(
    std::uint32_t index, ade::NodeHandle nodeHandle,
    std::uint32_t outputChannelIndex)
{
    auto ret = false;
    if(index < count(ChannelListType::RegularList)
        && channelInfo_[index].channelType == ChannelType::Instrument
        && nodeHandle != nullptr
        && inputDevices_[index].second == nullptr)
    {
        auto device = graph_.getNodeData(nodeHandle).process.device();
        auto polarityInverterNode = polarityInverters_[index].second;
        if(outputChannelIndex < device->audioOutputGroupCount())
        {
            ret = graph_.connect(nodeHandle, polarityInverterNode, outputChannelIndex, 0).has_value();
            if(ret)
            {
                inputDevices_[index].second = nodeHandle;
                instrumentBypassed_[index] = false;
                instrumentOutputChannelIndex_[index] = outputChannelIndex;
                auto& auxInputIDs = pluginAuxInputs_
                    [ChannelListType::RegularList]
                    [index].first;
                auto& auxOutputIDs = pluginAuxOutputs_
                    [ChannelListType::RegularList]
                    [index].first;
                auto& auxInputSources = pluginAuxInputSources_
                    [ChannelListType::RegularList]
                    [index].first;
                auto& auxOutputDestinations = pluginAuxOutputDestinations_
                    [ChannelListType::RegularList]
                    [index].first;
                auxInputSources.resize(
                    device->audioInputGroupCount(),
                    Position { .type = Position::Type::Invalid }
                );
                auxOutputDestinations.resize(
                    device->audioOutputGroupCount() - 1
                );
                auxInputIDs.reserve(device->audioInputGroupCount());
                FOR_RANGE0(i, device->audioInputGroupCount())
                {
                    auxInputIDs.emplace_back(
                        pluginAuxInputIDs_.emplace(
                            auxInputIdGen_(),
                            PluginAuxIOPosition {
                                .channelListType = ChannelListType::RegularList,
                                .channelIndex = index,
                                .inChannelPosition = PluginAuxIOPosition::InChannelPosition::Instrument,
                                .isPreFaderInsert = false,
                                .insertIndex = 0U,
                                .channelGroupIndex = i
                            }
                        ).first
                    );
                }
                auxOutputIDs.reserve(device->audioOutputGroupCount() - 1);
                auto auxOutputChannelGroups = {
                    std::ranges::iota_view(0U, outputChannelIndex),
                    std::ranges::iota_view(outputChannelIndex + 1, device->audioOutputGroupCount())
                };
                for(auto i: std::ranges::join_view(auxOutputChannelGroups))
                {
                    auxOutputIDs.emplace_back(
                        pluginAuxOutputIDs_.emplace(
                            auxOutputIdGen_(),
                            PluginAuxIOPosition {
                                .channelListType = ChannelListType::RegularList,
                                .channelIndex = index,
                                .inChannelPosition = PluginAuxIOPosition::InChannelPosition::Instrument,
                                .isPreFaderInsert = false,
                                .insertIndex = 0U,
                                .channelGroupIndex = i
                            }
                        ).first
                    );
                }
            }
        }
    }
    return ret;
}

bool Mixer::setInstrument(
    std::uint32_t index, ade::NodeHandle nodeHandle, Context&& context,
    std::uint32_t outputChannelIndex)
{
    auto ret = setInstrument(index, nodeHandle, outputChannelIndex);
    if(ret)
    {
        instrumentContexts_[index] = std::move(context);
    }
    return ret;
}

std::pair<ade::NodeHandle, Context> Mixer::detachInstrument(std::uint32_t index)
{
    if(index < count(ChannelListType::RegularList)
        && inputDevices_[index].second != nullptr
        && channelInfo_[index].channelType == ChannelType::Instrument)
    {
        auto instrumentNode = inputDevices_[index].second;
        auto polarityInverterNode = polarityInverters_[index].second;
        graph_.disconnect(polarityInverterNode->inEdges().front());
        inputDevices_[index].second = nullptr;
        auto& auxInputIDs = pluginAuxInputs_
            [ChannelListType::RegularList]
            [index].first;
        auto& auxOutputIDs = pluginAuxOutputs_
            [ChannelListType::RegularList]
            [index].first;
        auto& auxInputSources = pluginAuxInputSources_
            [ChannelListType::RegularList]
            [index].first;
        auto& auxOutputDestinations = pluginAuxOutputDestinations_
            [ChannelListType::RegularList]
            [index].first;
        auxInputIDs.clear(); auxInputIDs.shrink_to_fit();
        auxOutputIDs.clear(); auxOutputIDs.shrink_to_fit();
        auxInputSources.clear(); auxInputSources.shrink_to_fit();
        auxOutputDestinations.clear(); auxOutputDestinations.shrink_to_fit();
        return std::make_pair(
            std::move(instrumentNode),
            std::move(instrumentContexts_[index])
        );
    }
    return std::make_pair(ade::NodeHandle(), YADAW::Util::createUniquePtr(nullptr));
}

std::optional<IDGen::ID> Mixer::instrumentAuxInputID(
    std::uint32_t channelIndex, std::uint32_t channelGroupIndex) const
{
    if(channelIndex < count(ChannelListType::RegularList)
        && channelInfo_[channelIndex].channelType == ChannelType::Instrument
        && inputDevices_[channelIndex].second != nullptr
        && channelGroupIndex < graph_.getNodeData(inputDevices_[channelIndex].second).process.device()->audioInputGroupCount())
    {
        return pluginAuxInputs_
            [ChannelListType::RegularList]
            [channelIndex].first[channelGroupIndex]->first;
    }
    return std::nullopt;
}

std::optional<IDGen::ID> Mixer::instrumentAuxOutputID(
    std::uint32_t channelIndex, std::uint32_t channelGroupIndex) const
{
    if(channelIndex < count(ChannelListType::RegularList)
        && channelInfo_[channelIndex].channelType == ChannelType::Instrument
        && inputDevices_[channelIndex].second != nullptr
        && channelGroupIndex < graph_.getNodeData(inputDevices_[channelIndex].second).process.device()->audioOutputGroupCount())
    {
        if(channelGroupIndex > preFaderInserts_[channelIndex]->outChannelGroupIndex())
        {
            return pluginAuxOutputs_
                [ChannelListType::RegularList]
                [channelIndex].first[channelGroupIndex]->first;
        }
        if(channelGroupIndex <= preFaderInserts_[channelIndex]->outChannelGroupIndex())
        {
            return pluginAuxOutputs_
                [ChannelListType::RegularList]
                [channelIndex].first[channelGroupIndex - 1]->first;
        }
    }
    return std::nullopt;
}

bool Mixer::isInstrumentBypassed(std::uint32_t index) const
{
    if(index < count(ChannelListType::RegularList))
    {
        return instrumentBypassed_[index];
    }
    return false;
}

void Mixer::setInstrumentBypass(std::uint32_t index, bool bypass)
{
    if(index < count(ChannelListType::RegularList)
        && channelInfo_[index].channelType == ChannelType::Instrument
        && inputDevices_[index].second != nullptr)
    {
        if(instrumentBypassed_[index] != bypass)
        {
            if(bypass)
            {
                for(const auto& edge: inputDevices_[index].second->outEdges())
                {
                    if(edge->dstNode() == polarityInverters_[index].second)
                    {
                        graph_.disconnect(edge);
                        break;
                    }
                }
            }
            else
            {
                graph_.connect(
                    inputDevices_[index].second, polarityInverters_[index].second,
                    instrumentOutputChannelIndex_[index], 0
                );
            }
            instrumentBypassed_[index] = bypass;
        }
    }
}

void Mixer::setConnectionUpdatedCallback(
    ConnectionUpdatedCallback* callback)
{
    connectionUpdatedCallback_ = callback;
}

void Mixer::resetConnectionUpdatedCallback()
{
    connectionUpdatedCallback_ = &Impl::blankConnectionUpdatedCallback;
}

void Mixer::connectionUpdated() const
{
    connectionUpdatedCallback_(*this);
}

void Mixer::setPreInsertAudioInputChannelCallback(
    std::function<PreInsertChannelCallback>&& callback)
{
    preInsertAudioInputChannelCallback_ = std::move(callback);
}

void Mixer::setPreInsertRegularChannelCallback(std::function<PreInsertChannelCallback>&& callback)
{
    preInsertRegularChannelCallback_ = std::move(callback);
}

void Mixer::setPreInsertAudioOutputChannelCallback(std::function<PreInsertChannelCallback>&& callback)
{
    preInsertAudioOutputChannelCallback_ = std::move(callback);
}

void Mixer::resetPreInsertAudioInputChannelCallback()
{
    preInsertAudioInputChannelCallback_ = &Impl::blankPreInsertChannelCallback;
}

void Mixer::resetPreInsertRegularChannelCallback()
{
    preInsertRegularChannelCallback_ = &Impl::blankPreInsertChannelCallback;
}

void Mixer::resetPreInsertAudioOutputChannelCallback()
{
    preInsertAudioOutputChannelCallback_ = &Impl::blankPreInsertChannelCallback;
}

void Mixer::setSendAddedCallback(std::function<SendAddedCallback>&& callback)
{
    sendAddedCallback_ = std::move(callback);
}

void Mixer::setSendDestinationChangedCallback(std::function<SendDestinationChangedCallback>&& callback)
{
    sendDestinationChangedCallback_ = std::move(callback);
}

void Mixer::setSendRemovedCallback(std::function<SendRemovedCallback>&& callback)
{
    sendRemovedCallback_ = std::move(callback);
}

void Mixer::resetSendAddedCallback()
{
    sendAddedCallback_ = &Impl::blankSendAddedCallback;
}

void Mixer::resetSendDestinationChangedCallback()
{
    sendDestinationChangedCallback_ = &Impl::blankSendDestinationChangedCallback;
}

void Mixer::resetSendRemovedCallback()
{
    sendRemovedCallback_ = &Impl::blankSendRemovedCallback;
}

void Mixer::setAuxInputChangedCallback(std::function<AuxInputChangedCallback>&& callback)
{
    auxInputChangedCallback_ = std::move(callback);
}

void Mixer::setAuxOutputAddedCallback(std::function<AuxOutputAddedCallback>&& callback)
{
    auxOutputAddedCallback_ = std::move(callback);
}

void Mixer::setAuxOutputDestinationChangedCallback(std::function<AuxOutputDestinationChangedCallback>&& callback)
{
    auxOutputDestinationChangedCallback_ = std::move(callback);
}

void Mixer::setAuxOutputRemovedCallback(std::function<AuxOutputRemovedCallback>&& callback)
{
    auxOutputRemovedCallback_ = std::move(callback);
}

void Mixer::resetAuxInputChangedCallback()
{
    auxInputChangedCallback_ = &Impl::blankAuxInputChangedCallback;
}

void Mixer::resetAuxOutputAddedCallback()
{
    auxOutputAddedCallback_ = &Impl::blankAuxOutputAddedCallback;
}

void Mixer::resetAuxOutputDestinationChangedCallback()
{
    auxOutputDestinationChangedCallback_ = &Impl::blankAuxOutputDestinationChangedCallback;
}

void Mixer::resetAuxOutputRemovedCallback()
{
    auxOutputRemovedCallback_ = &Impl::blankAuxOutputRemovedCallback;
}

OptionalRef<YADAW::Util::BatchUpdater> Mixer::batchUpdater()
{
    if(batchUpdater_)
    {
        return {*batchUpdater_};
    }
    return std::nullopt;
}

void Mixer::setBatchUpdater(YADAW::Util::BatchUpdater& batchUpdater)
{
    batchUpdater_ = &batchUpdater;
    auto vecs = {
        std::ranges::ref_view(audioInputPreFaderInserts_),
        std::ranges::ref_view(audioInputPostFaderInserts_),
        std::ranges::ref_view(preFaderInserts_),
        std::ranges::ref_view(postFaderInserts_),
        std::ranges::ref_view(audioOutputPreFaderInserts_),
        std::ranges::ref_view(audioOutputPostFaderInserts_)
    };
    auto view = std::ranges::views::join(vecs);
    for(auto& inserts: view)
    {
        inserts->setBatchUpdater(*batchUpdater_);
    }
}

void Mixer::resetBatchUpdater()
{
    batchUpdater_ = nullptr;
    auto vecs = {
        std::ranges::ref_view(audioInputPreFaderInserts_),
        std::ranges::ref_view(audioInputPostFaderInserts_),
        std::ranges::ref_view(preFaderInserts_),
        std::ranges::ref_view(postFaderInserts_),
        std::ranges::ref_view(audioOutputPreFaderInserts_),
        std::ranges::ref_view(audioOutputPostFaderInserts_)
    };
    auto view = std::ranges::views::join(vecs);
    for(auto& inserts: view)
    {
        inserts->resetBatchUpdater();
    }
}

std::optional<Mixer::Position> Mixer::getAuxInputSource(const PluginAuxIOPosition& position) const
{
    if(position.channelIndex < count(position.channelListType))
    {
        if(position.inChannelPosition == PluginAuxIOPosition::InChannelPosition::Instrument
            && channelInfos_[position.channelListType][position.channelIndex].channelType == ChannelType::Instrument
            && position.channelGroupIndex < graph_.getNodeData(
                inputDevices_[position.channelIndex].second
            ).process.device()->audioInputGroupCount()
        )
        {
            return const_cast<Mixer&>(*this).getAuxInputSource(position);
        }
        else if(position.inChannelPosition == PluginAuxIOPosition::InChannelPosition::Inserts)
        {
            if(
                const auto& inserts = (position.isPreFaderInsert?
                    preFaderInsertsAt(position.channelListType, position.channelIndex):
                    postFaderInsertsAt(position.channelListType, position.channelIndex)
                )->get();
                position.insertIndex < inserts.insertCount() && position.channelGroupIndex < graph_.getNodeData(
                    *inserts.insertNodeAt(position.insertIndex)
                ).process.device()->audioInputGroupCount()
            )
            {
                return const_cast<Mixer&>(*this).getAuxInputSource(position);
            }
        }
    }
    return std::nullopt;
}

bool Mixer::setAuxInputSource(const PluginAuxIOPosition& position, Position source)
{
    // TODO
    auto oldSource = getAuxInputSource(position);
    if(oldSource == source)
    {
        return true;
    }
    // Disconnect
    if(oldSource.type == Position::Type::Send)
    {
        const auto& sendPosition = sendPositions_.find(oldSource.id)->second;
        removeSend(sendPosition.channelListType, sendPosition.channelIndex, sendPosition.sendIndex);
    }
    else if(oldSource.type == Position::Type::PluginAuxIO)
    {
        auto auxOutput = *getAuxOutputPosition(source.id);
        auto& dests = getAuxOutputDestinations(auxOutput);
        FOR_RANGE0(i, dests.size())
        {
            if(const auto& dest = dests[i]; dest.type == Position::Type::PluginAuxIO && dest.id == getAuxInputPositionID(position))
            {
                removeAuxOutputDestination(auxOutput, i);
                break;
            }
        }
    }
    else if(oldSource.type == Position::Type::Send)
    {
        const auto& sendPos = sendPositions_.find(source.id)->second;
        removeSend(sendPos.channelListType, sendPos.channelIndex, sendPos.sendIndex);
    }
    else if(oldSource.type == Position::Type::SendAndFXChannel)
    {
        auto channelIndex = std::lower_bound(
            channelIdAndIndex_.begin(),
            channelIdAndIndex_.end(),
            source.id, &compareIdAndIndexWithId
        )->index;
        return setMainOutputAt(channelIndex, {});
    }
    // Connect
    if(source.type == Position::Type::Send)
    {
        const auto& sendPos = sendPositions_.find(source.id)->second;
        return setSendDestination(
            sendPos.channelListType, sendPos.channelIndex, sendPos.sendIndex,
            Position {.type = Position::Type::PluginAuxIO, .id = getAuxInputPositionID(position)}
        ).value_or(false);
    }
    else if(source.type == Position::Type::PluginAuxIO)
    {
        auto auxOutput = *getAuxOutputPosition(source.id);
        return addAuxOutputDestination(
            auxOutput,
            Position {.type = Position::Type::PluginAuxIO, .id = getAuxInputPositionID(position)}
        );
    }
    else if(source.type == Position::Type::SendAndFXChannel)
    {
        auto channelIndex = std::lower_bound(
            channelIdAndIndex_.begin(),
            channelIdAndIndex_.end(),
            source.id, &compareIdAndIndexWithId
        )->index;
        return setMainOutputAt(
            channelIndex,
            Position {.type = Position::Type::PluginAuxIO, .id = getAuxInputPositionID(position)}
        );
    }
    else if(source.type == Position::Type::Invalid)
    {
        return true;
    }
    return false;
}

OptionalRef<const std::vector<Mixer::Position>> Mixer::getAuxOutputDestinations(const PluginAuxIOPosition& position) const
{
    if(position.channelIndex < count(position.channelListType))
    {
        if(position.inChannelPosition == PluginAuxIOPosition::InChannelPosition::Instrument
            && channelInfos_[position.channelListType][position.channelIndex].channelType == ChannelType::Instrument
            && position.channelGroupIndex < graph_.getNodeData(
                inputDevices_[position.channelIndex].second
            ).process.device()->audioOutputGroupCount()
        )
        {
            return const_cast<Mixer&>(*this).getAuxOutputDestinations(position);
        }
        else if(position.inChannelPosition == PluginAuxIOPosition::InChannelPosition::Inserts)
        {
            if(
                const auto& inserts = (position.isPreFaderInsert?
                    preFaderInsertsAt(position.channelListType, position.channelIndex):
                    postFaderInsertsAt(position.channelListType, position.channelIndex)
                )->get();
                position.insertIndex < inserts.insertCount() && position.channelGroupIndex < graph_.getNodeData(
                    *inserts.insertNodeAt(position.insertIndex)
                ).process.device()->audioOutputGroupCount()
            )
            {
                return const_cast<Mixer&>(*this).getAuxOutputDestinations(position);
            }
        }
    }
    return std::nullopt;
}

bool Mixer::addAuxOutputDestination(const PluginAuxIOPosition& position, Position destination)
{
    auto ret = false;
    auto& destinations = getAuxOutputDestinations(position);
    if(std::ranges::find(destinations, destination) == destinations.end())
    {
        auto fromNode = getNodeFromPluginAuxPosition(position);
        auto device = graph_.getNodeData(fromNode).process.device();
        auto& fromNodeChannelGroup = device->audioOutputGroupAt(position.channelGroupIndex)->get();
        if(destination.type == Position::Type::AudioHardwareIOChannel)
        {
            auto it = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                destination.id, &compareIdAndIndexWithId
            );
            if(it != audioOutputChannelIdAndIndex_.end())
            {
                auto [destChannelGroupType, destChannelCountInGroup] = *channelGroupTypeAndChannelCountAt(ChannelListType::AudioHardwareOutputList, it->index);
                if(fromNodeChannelGroup.type() == destChannelGroupType && fromNodeChannelGroup.channelCount() == destChannelCountInGroup
                    && !YADAW::Util::pathExists(audioOutputSummings_[it->index].second, fromNode))
                {
                    auto& oldSummingAndNode = audioOutputSummings_[it->index];
                    SummingAndNode newSummingAndNode = appendInputGroup(oldSummingAndNode);
                    graph_.connect(fromNode, newSummingAndNode.second, position.channelGroupIndex, newSummingAndNode.first->audioInputGroupCount() - 1);
                    const auto& polarityInverterNode = audioOutputPolarityInverters_[it->index].second;
                    graph_.disconnect(polarityInverterNode->inEdges().front());
                    graph_.connect(newSummingAndNode.second, polarityInverterNode, 0, 0);
                    if(batchUpdater_)
                    {
                        batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                    }
                    else
                    {
                        connectionUpdatedCallback_(*this);
                    }
                    std::swap(oldSummingAndNode, newSummingAndNode);
                    ret = true;
                }
            }
        }
        else if(destination.type == Position::Type::SendAndFXChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                destination.id, &compareIdAndIndexWithId
            );
            if(it != audioOutputChannelIdAndIndex_.end()
                && (channelInfo_[it->index].channelType == ChannelType::AudioFX || channelInfo_[it->index].channelType == ChannelType::AudioBus)
            )
            {
                auto [destChannelGroupType, destChannelCountInGroup] = *channelGroupTypeAndChannelCountAt(ChannelListType::RegularList, it->index);
                if(fromNodeChannelGroup.type() == destChannelGroupType && fromNodeChannelGroup.channelCount() == destChannelCountInGroup
                    && !YADAW::Util::pathExists(inputDevices_[it->index].second, fromNode))
                {
                    auto& oldSummingAndNodeAsDevice = inputDevices_[it->index];
                    SummingAndNode newSummingAndNode = appendInputGroup(oldSummingAndNodeAsDevice);
                    graph_.connect(fromNode, newSummingAndNode.second, position.channelGroupIndex, newSummingAndNode.first->audioInputGroupCount() - 1);
                    const auto& polarityInverterNode = polarityInverters_[it->index].second;
                    graph_.disconnect(polarityInverterNode->inEdges().front());
                    graph_.connect(newSummingAndNode.second, polarityInverterNode, 0, 0);
                    preFaderInserts_[it->index]->setInNode(newSummingAndNode.second, 0);
                    if(batchUpdater_)
                    {
                        batchUpdater_->addObject(std::move(oldSummingAndNodeAsDevice.first));
                    }
                    else
                    {
                        connectionUpdatedCallback_(*this);
                    }
                    oldSummingAndNodeAsDevice.first = std::move(newSummingAndNode.first);
                    std::swap(oldSummingAndNodeAsDevice.second, newSummingAndNode.second);
                    ret = true;
                }
            }
        }
        else if(destination.type == Position::Type::AudioChannel)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                destination.id, &compareIdAndIndexWithId
            );
            if(it != audioOutputChannelIdAndIndex_.end()
                && channelInfo_[it->index].channelType == ChannelType::Audio
                && mainInput_[it->index].type == Position::Type::Invalid)
            {
                auto [destChannelGroupType, destChannelCountInGroup] = *channelGroupTypeAndChannelCountAt(ChannelListType::RegularList, it->index);
                if(fromNodeChannelGroup.type() == destChannelGroupType && fromNodeChannelGroup.channelCount() == destChannelCountInGroup
                    && !YADAW::Util::pathExists(inputDevices_[it->index].second, fromNode))
                {
                    graph_.connect(fromNode, inputDevices_[it->index].second, position.channelGroupIndex, 1);
                    mainInput_[it->index] = Position {
                        .type = Position::Type::PluginAuxIO,
                        .id = getAuxOutputPositionID(position)
                    };
                    ret = true;
                }
            }
        }
        else if(destination.type == Position::Type::PluginAuxIO)
        {
            auto auxInput = *getAuxInputPosition(destination.id);
            if(auto& auxInputSource = getAuxInputSource(auxInput);
                auxInputSource.type == Position::Type::Invalid)
            {
                auto toNode = getNodeFromPluginAuxPosition(auxInput);
                auto& toNodeChannelGroup = graph_.getNodeData(toNode).process.device()->audioInputGroupAt(auxInput.channelGroupIndex)->get();
                if(fromNodeChannelGroup.type() == toNodeChannelGroup.type()
                    && fromNodeChannelGroup.channelCount() == toNodeChannelGroup.channelCount())
                {
                    if(auto edgeHandle = graph_.connect(fromNode, toNode, position.channelGroupIndex, auxInput.channelGroupIndex))
                    {
                        const auto& _po1 = pluginAuxOutputs_[position.channelListType][position.channelIndex];
                        const auto& _po2 = position.inChannelPosition == PluginAuxIOPosition::InChannelPosition::Instrument?
                            _po1.first:
                            _po1.second[position.isPreFaderInsert? 0: 1][position.insertIndex];
                        auxInputSource = Position {
                            .type = Position::Type::PluginAuxIO,
                            .id = getAuxOutputPositionID(position)
                        };
                        ret = true;
                    }
                }
            }
        }
        if(ret)
        {
            connectionUpdatedCallback_(*this);
            destinations.emplace_back(destination);
        }
    }
    return ret;
}

bool Mixer::setAuxOutputDestination(const PluginAuxIOPosition& position, std::uint32_t index, Position destination)
{
    return false;
}

bool Mixer::removeAuxOutputDestination(const PluginAuxIOPosition& position, std::uint32_t index,
    std::uint32_t removeCount)
{
    auto& destinations = getAuxOutputDestinations(position);
    if(index < destinations.size() && removeCount > 0 && index + removeCount <= destinations.size())
    {
        auto fromNode = getNodeFromPluginAuxPosition(position);
        auto removingDestinations = std::ranges::views::counted(destinations.begin() + index, removeCount);
        std::vector<Context> removingObjects;
        removingObjects.reserve(
            std::ranges::count_if(
                removingDestinations,
                [](const auto& destination)
                {
                    return destination.type == Position::Type::AudioHardwareIOChannel
                        || destination.type == Position::Type::SendAndFXChannel;
                }
            )
        );
        for(const auto& dest: removingDestinations)
        {
            if(dest.type == Position::Type::AudioHardwareIOChannel)
            {
                auto it = std::lower_bound(
                    audioOutputChannelIdAndIndex_.begin(),
                    audioOutputChannelIdAndIndex_.end(),
                    dest.id, &compareIdAndIndexWithId
                );
                auto& oldSummingAndNode = audioOutputSummings_[it->index];
                for(const auto& inEdge: oldSummingAndNode.second->inEdges())
                {
                    if(inEdge->srcNode() == fromNode)
                    {
                        if(const auto& edgeData = graph_.getEdgeData(inEdge);
                            edgeData.fromChannel == position.channelGroupIndex)
                        {
                            auto toChannel = edgeData.toChannel;
                            auto newSummingAndNode = *removeInputGroup(oldSummingAndNode, toChannel);
                            const auto& polarityInverterNode = audioOutputPolarityInverters_[it->index].second;
                            graph_.disconnect(polarityInverterNode->inEdges().front());
                            graph_.connect(newSummingAndNode.second, polarityInverterNode, 0, 0);
                            graph_.disconnect(inEdge);
                            removingObjects.emplace_back(
                                YADAW::Util::createPMRUniquePtr(std::move(oldSummingAndNode.first))
                            );
                            if(auto multiInput = graphWithPDC_.removeNode(oldSummingAndNode.second))
                            {
                                removingObjects.emplace_back(
                                    YADAW::Util::createPMRUniquePtr(std::move(multiInput))
                                );
                            }
                            if(batchUpdater_)
                            {
                                for(auto& removingObject: removingObjects)
                                {
                                    batchUpdater_->addObject(std::move(oldSummingAndNode.first));
                                }
                            }
                            oldSummingAndNode = std::move(newSummingAndNode);
                            break;
                        }
                    }
                }
            }
            else if(dest.type == Position::Type::SendAndFXChannel)
            {
                auto it = std::lower_bound(
                    channelIdAndIndex_.begin(),
                    channelIdAndIndex_.end(),
                    dest.id, &compareIdAndIndexWithId
                );
                auto& oldSummingAndNodeAsDevice = inputDevices_[it->index];
                for(const auto& inEdge: oldSummingAndNodeAsDevice.second->inEdges())
                {
                    if(inEdge->srcNode() == fromNode)
                    {
                        if(const auto& edgeData = graph_.getEdgeData(inEdge);
                            edgeData.fromChannel == position.channelGroupIndex)
                        {
                            auto toChannel = edgeData.toChannel;
                            auto newSummingAndNode = *removeInputGroup(oldSummingAndNodeAsDevice, toChannel);
                            const auto& polarityInverterNode = polarityInverters_[it->index].second;
                            graph_.disconnect(polarityInverterNode->inEdges().front());
                            graph_.connect(newSummingAndNode.second, polarityInverterNode, 0, 0);
                            graph_.disconnect(inEdge);
                            removingObjects.emplace_back(
                                YADAW::Util::createPMRUniquePtr(std::move(oldSummingAndNodeAsDevice.first))
                            );
                            if(auto multiInput = graphWithPDC_.removeNode(oldSummingAndNodeAsDevice.second))
                            {
                                removingObjects.emplace_back(
                                    YADAW::Util::createPMRUniquePtr(std::move(multiInput))
                                );
                            }
                            if(batchUpdater_)
                            {
                                for(auto& removingObject: removingObjects)
                                {
                                    batchUpdater_->addObject(std::move(oldSummingAndNodeAsDevice.first));
                                }
                            }
                            oldSummingAndNodeAsDevice = std::move(newSummingAndNode);
                            break;
                        }
                    }
                }
            }
            else if(dest.type == Position::Type::AudioChannel)
            {
                auto it = std::lower_bound(
                    channelIdAndIndex_.begin(),
                    channelIdAndIndex_.end(),
                    dest.id, &compareIdAndIndexWithId
                );
                auto& [input, destNode] = inputDevices_[it->index];
                auto inEdges = destNode->inEdges();
                for(const auto& inEdge: inEdges)
                {
                    if(graph_.getEdgeData(inEdge).toChannel == 1)
                    {
                        graph_.disconnect(inEdge);
                        break;
                    }
                }
            }
            else if(dest.type == Position::Type::PluginAuxIO)
            {
                auto auxInput = *getAuxInputPosition(dest.id);
                auto toNode = getNodeFromPluginAuxPosition(auxInput);
                getAuxInputSource(auxInput) = {};
            }
        }
        if(!batchUpdater_)
        {
            connectionUpdatedCallback_(*this);
        }
        destinations.erase(
            destinations.begin() + index,
            destinations.begin() + index + removeCount
        );
        return true;
    }
    return false;
}

void Mixer::clearAuxOutputDestinations(const PluginAuxIOPosition& position)
{
    auto& destinations = getAuxOutputDestinations(position);
    removeAuxOutputDestination(position, 0, destinations.size());
}

std::tuple<Mixer::PolarityInverterAndNode, Mixer::MuteAndNode, Mixer::FaderAndNode> Mixer::createSend(
    ade::NodeHandle fromNode, ade::NodeHandle toNode,
    std::uint32_t fromChannel, std::uint32_t toChannel)
{
    auto& channelGroup = graph_.getNodeData(fromNode).process.device()->audioOutputGroupAt(fromChannel)->get();
    auto channelGroupType = channelGroup.type();
    auto channelCountInGroup = channelGroup.channelCount();
    auto polarityInverter = std::make_unique<PolarityInverter>(channelGroupType, channelCountInGroup);
    auto polarityInverterNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*polarityInverter));
    auto mute = std::make_unique<YADAW::Audio::Util::Mute>(channelGroupType, channelCountInGroup);
    auto muteNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*mute));
    auto fader = volumeFaderFactory_(channelGroupType, channelCountInGroup);
    auto faderNode = graph_.addNode(YADAW::Audio::Engine::AudioDeviceProcess(*fader));
    graph_.connect(fromNode, polarityInverterNode, fromChannel, 0);
    graph_.connect(polarityInverterNode, muteNode, 0, 0);
    graph_.connect(muteNode, faderNode, 0, 0);
    graph_.connect(faderNode, toNode, 0, toChannel);
    return {
        std::pair{std::move(polarityInverter), std::move(polarityInverterNode)},
        std::pair{std::move(mute), std::move(muteNode)},
        std::pair{std::move(fader), std::move(faderNode)}
    };
}

std::optional<Mixer::SendPosition> Mixer::getSendPosition(IDGen::ID id) const
{
    if(auto it = sendPositions_.find(id); it != sendPositions_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

std::optional<Mixer::PluginAuxIOPosition> Mixer::getAuxInputPosition(IDGen::ID id) const
{
    auto it = pluginAuxInputIDs_.find(id);
    if(it != pluginAuxInputIDs_.end())
    {
        return {it->second};
    }
    return std::nullopt;
}

std::optional<Mixer::PluginAuxIOPosition> Mixer::getAuxOutputPosition(IDGen::ID id) const
{
    auto it = pluginAuxOutputIDs_.find(id);
    if(it != pluginAuxOutputIDs_.end())
    {
        return {it->second};
    }
    return std::nullopt;
}

IDGen::ID Mixer::getAuxInputPositionID(const PluginAuxIOPosition& position) const
{
    auto& v = pluginAuxInputs_[position.channelListType][position.channelIndex];
    return (
        position.inChannelPosition == PluginAuxIOPosition::InChannelPosition::Instrument?
        v.first[position.channelGroupIndex]:
        v.second[position.isPreFaderInsert? 0: 1][position.insertIndex]
        [position.channelGroupIndex - (
            position.channelGroupIndex > (
                (position.isPreFaderInsert? channelPreFaderInserts_: channelPostFaderInserts_)
                [position.channelListType][position.channelIndex]
                ->insertInputChannelGroupIndexAt(position.insertIndex).value()
            )
        )]
    )->first;
}

IDGen::ID Mixer::getAuxOutputPositionID(const PluginAuxIOPosition& position) const
{
    auto& v = pluginAuxOutputs_[position.channelListType][position.channelIndex];
    return (
        position.inChannelPosition == PluginAuxIOPosition::InChannelPosition::Instrument?
        v.first[position.channelGroupIndex - (position.channelGroupIndex > instrumentOutputChannelIndex_[position.channelIndex])]:
        v.second[position.isPreFaderInsert? 1: 0][position.insertIndex]
        [position.channelGroupIndex - (
            position.channelGroupIndex > (
                (position.isPreFaderInsert? channelPreFaderInserts_: channelPostFaderInserts_)
                [position.channelListType][position.channelIndex]
                ->insertOutputChannelGroupIndexAt(position.insertIndex).value()
            )
        )]
    )->first;
}

Mixer::Position& Mixer::getAuxInputSource(const PluginAuxIOPosition& position)
{
    auto& v1 = pluginAuxInputSources_[position.channelListType][position.channelIndex];
    if(position.inChannelPosition == PluginAuxIOPosition::InChannelPosition::Instrument)
    {
        return v1.first[position.channelGroupIndex];
    }
    else
    {
        auto& inserts = (
            position.isPreFaderInsert? channelPreFaderInserts_: channelPostFaderInserts_
        )[position.channelListType][position.channelIndex];
        auto afterMain = position.channelGroupIndex > *(inserts->insertInputChannelGroupIndexAt(position.insertIndex));
        auto index = position.channelGroupIndex - afterMain;
        return v1.second[position.isPreFaderInsert? 0: 1][position.insertIndex][index];
    }
}

std::vector<Mixer::Position>& Mixer::getAuxOutputDestinations(const PluginAuxIOPosition& position)
{
    auto& v1 = pluginAuxOutputDestinations_[position.channelListType][position.channelIndex];
    if(position.inChannelPosition == PluginAuxIOPosition::InChannelPosition::Instrument)
    {
        auto afterMain = position.channelGroupIndex > instrumentOutputChannelIndex_[position.channelIndex];
        auto index = position.channelGroupIndex - afterMain;
        return v1.first[index];
    }
    else
    {
        auto& inserts = (
            position.isPreFaderInsert? channelPreFaderInserts_: channelPostFaderInserts_
        )[position.channelListType][position.channelIndex];
        auto afterMain = position.channelGroupIndex > *(inserts->insertOutputChannelGroupIndexAt(position.insertIndex));
        auto index = position.channelGroupIndex - afterMain;
        return v1.second[position.isPreFaderInsert? 0: 1][position.insertIndex][index];
    }
}

void Mixer::insertAdded(Inserts& sender, std::uint32_t position)
{
    auto& insertPosition = *static_cast<InsertPosition*>(sender.getInsertCallbackUserData().get());
    auto& mixer = insertPosition.mixer;
    auto& vecInput = mixer.pluginAuxInputs_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    auto& vecOutput = mixer.pluginAuxOutputs_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    auto& vecInputSource = mixer.pluginAuxInputSources_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    auto& vecOutputDestinations = mixer.pluginAuxOutputDestinations_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    auto auxInputIt = vecInput.emplace(vecInput.begin() + position);
    auto auxOutputIt = vecOutput.emplace(vecOutput.begin() + position);
    auto auxInputSourceIt = vecInputSource.emplace(vecInputSource.begin() + position);
    auto auxOutputDestinationsIt = vecOutputDestinations.emplace(vecOutputDestinations.begin() + position);
    auto node = *sender.insertNodeAt(position);
    auto device = sender.graph().getNodeData(node).process.device();
    auxInputIt->reserve(device->audioInputGroupCount() - 1);
    auxOutputIt->reserve(device->audioOutputGroupCount() - 1);
    auxInputSourceIt->resize(device->audioInputGroupCount() - 1, Position {.type = Position::Type::Invalid, .id = IDGen::InvalidId});
    auxOutputDestinationsIt->resize(device->audioOutputGroupCount() - 1);
    auto inputIndex = *sender.insertInputChannelGroupIndexAt(position);
    auto outputIndex = *sender.insertOutputChannelGroupIndexAt(position);
    FOR_RANGE0(i, inputIndex)
    {
        auto id = *sender.insertAuxInputID(position, i);
        auxInputIt->emplace_back(
            mixer.pluginAuxInputIDs_.emplace(
                id, PluginAuxIOPosition {
                    .channelListType = insertPosition.type,
                    .channelIndex = insertPosition.channelIndex,
                    .inChannelPosition = PluginAuxIOPosition::InChannelPosition::Inserts,
                    .isPreFaderInsert = insertPosition.insertsIndex == 0,
                    .insertIndex = position,
                    .channelGroupIndex = i
                }
            ).first
        );
    }
    FOR_RANGE(i, inputIndex + 1, device->audioInputGroupCount())
    {
        auto id = *sender.insertAuxInputID(position, i);
        auxInputIt->emplace_back(
            mixer.pluginAuxInputIDs_.emplace(
                id, PluginAuxIOPosition {
                    .channelListType = insertPosition.type,
                    .channelIndex = insertPosition.channelIndex,
                    .inChannelPosition = PluginAuxIOPosition::InChannelPosition::Inserts,
                    .isPreFaderInsert = insertPosition.insertsIndex == 0,
                    .insertIndex = position,
                    .channelGroupIndex = i
                }
            ).first
        );
    }
    FOR_RANGE0(i, outputIndex)
    {
        auto id = *sender.insertAuxOutputID(position, i);
        auxInputIt->emplace_back(
            mixer.pluginAuxOutputIDs_.emplace(
                id, PluginAuxIOPosition {
                    .channelListType = insertPosition.type,
                    .channelIndex = insertPosition.channelIndex,
                    .inChannelPosition = PluginAuxIOPosition::InChannelPosition::Inserts,
                    .isPreFaderInsert = insertPosition.insertsIndex == 0,
                    .insertIndex = position,
                    .channelGroupIndex = i
                }
            ).first
        );
    }
    FOR_RANGE(i, outputIndex + 1, device->audioOutputGroupCount())
    {
        auto id = *sender.insertAuxOutputID(position, i);
        auxInputIt->emplace_back(
            mixer.pluginAuxOutputIDs_.emplace(
                id, PluginAuxIOPosition {
                    .channelListType = insertPosition.type,
                    .channelIndex = insertPosition.channelIndex,
                    .inChannelPosition = PluginAuxIOPosition::InChannelPosition::Inserts,
                    .isPreFaderInsert = insertPosition.insertsIndex == 0,
                    .insertIndex = position,
                    .channelGroupIndex = i
                }
            ).first
        );
    }
    FOR_RANGE(i, position + 1, vecInput.size())
    {
        for(auto it: vecInput[i])
        {
            it->second.insertIndex = i;
        }
    }
    FOR_RANGE(i, position + 1, vecOutput.size())
    {
        for(auto it: vecOutput[i])
        {
            it->second.insertIndex = i;
        }
    }
}

void Mixer::insertAboutToBeRemoved(Inserts& sender, std::uint32_t position, std::uint32_t removeCount)
{
    auto& insertPosition = *static_cast<InsertPosition*>(sender.getInsertCallbackUserData().get());
    auto& mixer = insertPosition.mixer;
    auto& vecInput = mixer.pluginAuxInputs_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    auto& vecOutput = mixer.pluginAuxOutputs_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    FOR_RANGE(i, position, position + removeCount)
    {
        for(auto it: vecInput[i])
        {
            mixer.setAuxInputSource(it->second, {});
        }
        for(auto it: vecOutput[i])
        {
            mixer.clearAuxOutputDestinations(it->second);
        }
    }
}

void Mixer::insertRemoved(Inserts& sender, std::uint32_t position, std::uint32_t removeCount)
{
    auto& insertPosition = *static_cast<InsertPosition*>(sender.getInsertCallbackUserData().get());
    auto& mixer = insertPosition.mixer;
    auto& vecInput = mixer.pluginAuxInputs_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    auto& vecOutput = mixer.pluginAuxOutputs_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    auto& vecInputSource = mixer.pluginAuxInputSources_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    auto& vecOutputDestinations = mixer.pluginAuxOutputDestinations_[insertPosition.type][insertPosition.channelIndex].second[insertPosition.insertsIndex];
    FOR_RANGE(i, position, position + removeCount)
    {
        for(auto it: vecInput[i])
        {
            mixer.pluginAuxInputIDs_.erase(it);
        }
        for(auto it: vecOutput[i])
        {
            mixer.pluginAuxOutputIDs_.erase(it);
        }
    }
    vecInput.erase(vecInput.begin() + position, vecInput.begin() + position + removeCount);
    vecOutput.erase(vecOutput.begin() + position, vecOutput.begin() + position + removeCount);
    vecInputSource.erase(vecInputSource.begin() + position, vecInputSource.begin() + position + removeCount);
    vecOutputDestinations.erase(vecOutputDestinations.begin() + position, vecOutputDestinations.begin() + position + removeCount);
    FOR_RANGE(i, position, vecInput.size())
    {
        for(auto it: vecInput[i])
        {
            it->second.insertIndex = i;
        }
    }
    FOR_RANGE(i, position, vecOutput.size())
    {
        for(auto it: vecOutput[i])
        {
            it->second.insertIndex = i;
        }
    }
}

void Mixer::updatePluginAuxPosition(
    ChannelListType type,
    std::uint32_t fromChannelIndex)
{
    auto& preFaderInserts = channelPreFaderInserts_[type];
    auto& postFaderInserts = channelPostFaderInserts_[type];
    auto& vecIn = pluginAuxInputs_[type];
    for(auto it = vecIn.begin() + fromChannelIndex; it != vecIn.end(); ++it)
    {
        auto channelIndex = std::distance(vecIn.begin(), it);
        for(auto& position: it->first)
        {
            position->second.channelIndex = channelIndex;
        }
        for(auto& inserts: it->second)
        {
            for(auto& insert: inserts)
            {
                for(auto& position: insert)
                {
                    position->second.channelIndex = channelIndex;
                }
            }
        }
    }
    auto& vecOut = pluginAuxOutputs_[type];
    for(auto it = vecOut.begin() + fromChannelIndex; it != vecOut.end(); ++it)
    {
        auto channelIndex = std::distance(vecOut.begin(), it);
        for(auto& position: it->first)
        {
            position->second.channelIndex = channelIndex;
        }
        for(auto& inserts: it->second)
        {
            for(auto& insert: inserts)
            {
                for(auto& position: insert)
                {
                    position->second.channelIndex = channelIndex;
                }
            }
        }
    }
    FOR_RANGE(i, fromChannelIndex, preFaderInserts.size())
    {
        static_cast<InsertPosition*>(preFaderInserts[i]->getInsertCallbackUserData().get())->channelIndex = i;
    }
    FOR_RANGE(i, fromChannelIndex, postFaderInserts.size())
    {
        static_cast<InsertPosition*>(postFaderInserts[i]->getInsertCallbackUserData().get())->channelIndex = i;
    }
}

ade::NodeHandle Mixer::getNodeFromPluginAuxPosition(const PluginAuxIOPosition& position) const
{
    if(position.inChannelPosition == PluginAuxIOPosition::Inserts)
    {
        const Vec<std::unique_ptr<Inserts>>* insertsCollection[3][2] = {
            {&channelPostFaderInserts_[AudioHardwareInputList],  &channelPreFaderInserts_[AudioHardwareInputList]},
            {&channelPostFaderInserts_[RegularList],             &channelPreFaderInserts_[RegularList]},
            {&channelPostFaderInserts_[AudioHardwareOutputList], &channelPreFaderInserts_[AudioHardwareOutputList]}
        };
        auto& inserts = (*insertsCollection[position.channelListType][position.isPreFaderInsert])[position.channelIndex];
        return *(inserts->insertNodeAt(position.insertIndex));
    }
    else if(position.inChannelPosition == PluginAuxIOPosition::Instrument)
    {
        return inputDevices_[position.channelIndex].second;
    }
    return {};
}
}
