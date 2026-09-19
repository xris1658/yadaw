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

void blankMainInputChangedCallback(const Mixer&, std::uint32_t) {}

void blankMainOutputChangedCallback(const Mixer&, std::uint32_t) {}

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

template<typename T1, typename T2>
void swapSummingAndNodeUnchecked(
    std::pair<std::unique_ptr<T1>, ade::NodeHandle>& lhs,
    std::pair<std::unique_ptr<T2>, ade::NodeHandle>& rhs
)
{
    if constexpr(std::is_same_v<T1, T2>)
    {
        std::swap(lhs, rhs);
    }
    else if constexpr(std::is_base_of_v<T1, T2>)
    {
        using Base = T1; using Derived = T2;
        Base* lptr = lhs.first.release();
        lhs.first.reset(rhs.first.release());
        rhs.first.reset(static_cast<Derived*>(lptr));
        std::swap(lhs.second, rhs.second);
    }
    else if constexpr(std::is_base_of_v<T2, T1>)
    {
        swapSummingAndNodeUnchecked(rhs, lhs);
    }
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
    mainInputChangedCallback_(&Impl::blankMainInputChangedCallback),
    mainOutputChangedCallback_(&Impl::blankMainOutputChangedCallback),
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
                auto& destinations = audioInputDestinations_[i];
                FOR_RANGE0(j, destinations.size())
                {
                    const auto& position = *destinations.begin();
                    if(position.type == Position::Type::AudioChannelInput)
                    {
                        auto channelIndex = std::lower_bound(
                            channelIdAndIndex_.begin(),
                            channelIdAndIndex_.end(),
                            position.id, &compareIdAndIndexWithId
                        )->index;
                        setMainInputAt(channelIndex, {});
                    }
                }
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
                auto& destinations = regularChannelInputSources_[i];
                FOR_RANGE0(j, destinations.size())
                {
                    const auto& position = *destinations.begin();
                    if(position.type == Position::Type::RegularChannelOutput)
                    {
                        auto channelIndex = std::lower_bound(
                            channelIdAndIndex_.begin(),
                            channelIdAndIndex_.end(),
                            position.id, &compareIdAndIndexWithId
                        )->index;
                        setMainOutputAt(channelIndex, {});
                    }
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
            FOR_RANGE(i, index, last)
            {
                auto& destinations = audioOutputSources_[i];
                FOR_RANGE0(j, destinations.size())
                {
                    const auto& position = *destinations.begin();
                    if(position.type == Position::Type::RegularChannelOutput)
                    {
                        auto channelIndex = std::lower_bound(
                            channelIdAndIndex_.begin(),
                            channelIdAndIndex_.end(),
                            position.id, &compareIdAndIndexWithId
                        )->index;
                        setMainOutputAt(channelIndex, {});
                    }
                }
            }
        }
        FOR_RANGE(i, index, last)
        {
            nodesToRemove.emplace_back(channelPolarityInverters_[type][i].second);
            nodesToRemove.emplace_back(channelMutes_[type][i].second);
            nodesToRemove.emplace_back(channelFaders_[type][i].second);
            nodesToRemove.emplace_back(channelMeters_[type][i].second);
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
        channelMultiIOTargets_[type].erase(
            channelMultiIOTargets_[type].begin() + index,
            channelMultiIOTargets_[type].begin() + last
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
        if(auto oldPosition = mainInput_[index]; oldPosition.type == Position::Type::AudioHardwareIOChannel)
        {
            auto channelIndex = std::lower_bound(
                audioInputChannelIdAndIndex_.begin(),
                audioInputChannelIdAndIndex_.end(),
                oldPosition.id, &compareIdAndIndexWithId
            )->index;
            audioInputDestinations_[channelIndex].erase(Position {
                .type = Position::Type::AudioChannelInput,
                .id = channelId_[index]
            });
        }
        else if(oldPosition.type == Position::Type::RegularChannelOutput)
        {
            // TODO
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
                // TODO
            }
            audioInputDestinations_[it->index].emplace(
                Position::Type::AudioChannelInput, index
            );
        }
        else if(position.type == Position::Type::RegularChannelOutput)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                position.id,
                &compareIdAndIndexWithId
            );
            // TODO
        }
        if(ret)
        {
            mainInput_[index] = position;
            mainInputChangedCallback_(*this, index);
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
        auto unsetMainOutput = mainOutput_[index].type == Position::Type::Invalid?
            YADAW::Util::RollbackableOperation():
            coUnsetMainOutput(index);
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
                    // TODO
                }
            }
        }
        else if(position.type == Position::Type::FXAndGroupChannelInput)
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
                        // TODO
                    }
                }
            }
        }
        else if(position.type == Position::Type::AudioChannelInput)
        {
            auto it = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                position.id,
                &compareIdAndIndexWithId
            );
            if(it != channelIdAndIndex_.end() && it->id == position.id)
            {
                // TODO
            }
        }
        else if(position.type == Position::Type::Invalid)
        {
            unsetMainOutput.commit();
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
        mainOutputChangedCallback_(*this, index);
    }
    return ret;
}

YADAW::Util::RollbackableOperation Mixer::coUnsetMainOutput(std::uint32_t index)
{
    bool shouldCommit = false;
    auto oldOutputDest = mainOutput_[index];
    auto optTask = createDisconnectTask(
        Position {
            .type = Position::Type::RegularChannelOutput,
            .id = channelId_[index]
        },
        oldOutputDest
    );
    co_yield shouldCommit;
    if(optTask)
    {
        optTask->setShouldCommit(shouldCommit);
        optTask.reset();
    }
    if(shouldCommit)
    {
        if(oldOutputDest.type == Position::Type::AudioChannelInput)
        {
            auto destIndex = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                oldOutputDest.id, &compareIdAndIndexWithId
            )->index;
            mainInput_[destIndex] = {};
            mainInputChangedCallback_(*this, destIndex);
            batchUpdateIfNeeded();
        }
        else if(oldOutputDest.type == Position::Type::FXAndGroupChannelInput)
        {
            auto destIndex = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                oldOutputDest.id, &compareIdAndIndexWithId
            )->index;
            auto& oldSummingAndNode = inputDevices_[destIndex];
            auto newSummingAndNode = shrinkInputGroups(oldSummingAndNode);
            auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second);
            graph_.connect(
                newSummingAndNode.second, polarityInverters_[destIndex].second, 0, 0
            );
            swapSummingAndNodeUnchecked(oldSummingAndNode, newSummingAndNode);
            batchUpdateIfNeeded(
                std::move(multiInputWithPDC), std::move(newSummingAndNode.first)
            );
            regularChannelInputSources_[destIndex].erase(Position {
                .type = Position::Type::RegularChannelOutput,
                .id = channelId_[index]
            });
        }
        else if(oldOutputDest.type == Position::Type::AudioHardwareIOChannel)
        {
            auto destIndex = std::lower_bound(
                audioOutputChannelIdAndIndex_.begin(),
                audioOutputChannelIdAndIndex_.end(),
                oldOutputDest.id, &compareIdAndIndexWithId
            )->index;
            auto& oldSummingAndNode = audioOutputSummings_[destIndex];
            auto newSummingAndNode = shrinkInputGroups(oldSummingAndNode);
            auto multiInputWithPDC = graphWithPDC_.removeNode(oldSummingAndNode.second);
            graph_.connect(
                newSummingAndNode.second, audioOutputPolarityInverters_[destIndex].second, 0, 0
            );
            swapSummingAndNodeUnchecked(oldSummingAndNode, newSummingAndNode);
            batchUpdateIfNeeded(
                std::move(multiInputWithPDC), std::move(newSummingAndNode.first)
            );
            audioOutputSources_[destIndex].erase(Position {
                .type = Position::Type::RegularChannelOutput,
                .id = channelId_[index]
            });
        }
        mainOutput_[index] = {};
        mainOutputChangedCallback_(*this, index);
    }
}

YADAW::Util::RollbackableOperation Mixer::coRemoveAudioHardwareInputChannelOutput(std::uint32_t index,
    Position destination)
{
    bool shouldCommit = false;
    auto optTask = createDisconnectTask(
        Position {
            .type = Position::Type::AudioHardwareIOChannel,
            .id = audioInputChannelId_[index]
        },
        destination
    );
    co_yield shouldCommit;
    if(optTask)
    {
        optTask->setShouldCommit(shouldCommit);
        optTask.reset();
    }
    if(shouldCommit)
    {
        audioInputDestinations_[index].erase(destination);
        if(destination.type == Position::Type::AudioChannelInput)
        {
            auto destIndex = std::lower_bound(
                channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                destination.id, &compareIdAndIndexWithId
            )->index;
            mainInput_[destIndex] = {};
            mainInputChangedCallback_(*this, destIndex);
            batchUpdateIfNeeded();
        }
        else if(destination.type == Position::Type::FXAndGroupChannelInput)
        {
            auto destIndex = std::lower_bound(
                channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
                destination.id, &compareIdAndIndexWithId
            )->index;
            auto& multiInput = inputDevices_[destIndex];
            std::optional<SummingAndNode> newMultiInput = std::nullopt;
            for(const auto& edge: multiInput.second->inEdges())
            {
                if(edge->srcNode() == audioInputMeters_[index].second)
                {
                    const auto& edgeData = graph_.getEdgeData(edge);
                    newMultiInput = removeInputGroup(multiInput, edgeData.toChannel);
                    break;
                }
            }
            assert(newMultiInput.has_value());
            auto pdc = graphWithPDC_.removeNode(multiInput.second);
            swapSummingAndNodeUnchecked(multiInput, *newMultiInput);
            auto& disposingMultiInput = *newMultiInput;
            auto connectNewMultiInput = graph_.connect(
                multiInput.second, polarityInverters_[destIndex].second, 0, 0
            );
            assert(connectNewMultiInput.has_value());
            batchUpdateIfNeeded(
                std::move(disposingMultiInput.first),
                std::move(pdc)
            );
            regularChannelInputSources_[destIndex].erase(
                Position {
                    .type = Position::Type::AudioHardwareIOChannel,
                    .id = audioInputChannelId_[index]
                }
            );
        }
    }
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
        audioInputMutes_.emplace(
            audioInputMutes_.begin() + position,
            std::move(mute), muteNode
        );
        graph_.connect(muteNode, faderNode, 0, 0);
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
        audioInputDestinations_.emplace(audioInputDestinations_.begin() + position);
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
        graph_.connect(polarityInverterNode, muteNode, 0, 0);
        graph_.connect(summingNode, polarityInverterNode, 0, 0);
        audioOutputMutes_.emplace(
            audioOutputMutes_.begin() + position,
            std::move(mute), muteNode
        );
        graph_.connect(muteNode, faderNode, 0, 0);
        graph_.connect(faderNode, meterNode, 0, 0);
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
        audioOutputSources_.emplace(audioOutputSources_.begin() + position);
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
    std::vector<DeviceAndNode<>> inputDevicesAndNode;
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
        std::fill_n(
            std::inserter(
                regularChannelInputSources_,
                regularChannelInputSources_.begin() + position
            ), count,
            std::decay_t<decltype(regularChannelInputSources_)>::value_type()
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
            Position {}
        );
        std::fill_n(
            std::inserter(mainOutput_, mainOutput_.begin() + position), count,
            Position {}
        );
        FOR_RANGE(i, position, position + count)
        {
            // TODO
        }
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
        return std::make_pair(
            std::move(instrumentNode),
            std::move(instrumentContexts_[index])
        );
    }
    return std::make_pair(ade::NodeHandle(), YADAW::Util::createUniquePtr(nullptr));
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

void Mixer::setMainInputChangedCallback(std::function<MainIOChangedCallback>&& callback)
{
    mainInputChangedCallback_ = std::move(callback);
}

void Mixer::setMainOutputChangedCallback(std::function<MainIOChangedCallback>&& callback)
{
    mainOutputChangedCallback_ = std::move(callback);
}

void Mixer::resetMainInputChangedCallback()
{
    mainInputChangedCallback_ = &Impl::blankMainInputChangedCallback;
}

void Mixer::resetMainOutputChangedCallback()
{
    mainOutputChangedCallback_ = &Impl::blankMainOutputChangedCallback;
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
    // TODO
}

void Mixer::resetBatchUpdater()
{
    batchUpdater_ = nullptr;
    // TODO
}

std::tuple<PolarityInverterAndNode, MuteAndNode, FaderAndNode> Mixer::createSend(
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

void Mixer::insertAdded(Inserts& sender, std::uint32_t position)
{
    // TODO
}

void Mixer::insertAboutToBeRemoved(Inserts& sender, std::uint32_t position, std::uint32_t removeCount)
{
    // TODO
}

void Mixer::insertRemoved(Inserts& sender, std::uint32_t position, std::uint32_t removeCount)
{
    // TODO
}

Mixer::DisconnectTask::DisconnectTask(): graph_(nullptr)
{
}

Mixer::DisconnectTask::DisconnectTask(Mixer& mixer, const ade::EdgeHandle& edgeHandle):
    graph_(&(mixer.graph_)),
    fromNode_(edgeHandle->srcNode()), toNode_(edgeHandle->dstNode())
{
    auto edgeData =graph_->getEdgeData(edgeHandle);
    fromChannel_ = edgeData.fromChannel;
    toChannel_ = edgeData.toChannel;
    graph_->disconnect(edgeHandle);
}

Mixer::DisconnectTask::DisconnectTask(DisconnectTask&& rhs) noexcept:
    graph_(rhs.graph_), fromNode_(rhs.fromNode_), toNode_(rhs.toNode_),
    fromChannel_(rhs.fromChannel_), toChannel_(rhs.toChannel_),
    shouldCommit_(rhs.shouldCommit_)
{
    rhs.graph_ = nullptr;
}

void Mixer::DisconnectTask::setShouldCommit(bool shouldCommit)
{
    shouldCommit_ = shouldCommit;
}

Mixer::DisconnectTask::~DisconnectTask()
{
    if(graph_ && !shouldCommit_)
    {
        graph_->connect(fromNode_, toNode_, fromChannel_, toChannel_);
    }
}

std::optional<Mixer::DisconnectTask> Mixer::createDisconnectTask(
    Mixer::Position source, Mixer::Position dest)
{
    assert(source.type != Position::Type::AudioChannelInput
        && source.type != Position::Type::FXAndGroupChannelInput);
    if(source.type == Position::Type::AudioHardwareIOChannel)
    {
        assert(dest.type != Mixer::Position::Type::AudioHardwareIOChannel
            && dest.type != Mixer::Position::Type::FXAndGroupChannelInput);
        auto itSrc = std::lower_bound(
            audioInputChannelIdAndIndex_.begin(),
            audioInputChannelIdAndIndex_.end(),
            source.id, &compareIdAndIndexWithId
        );
        assert(itSrc != audioInputChannelIdAndIndex_.end());
        auto srcNode = audioInputMeters_[itSrc->index].second;
        if(dest.type == Position::Type::AudioChannelInput)
        {
            auto itDest = std::lower_bound(
                channelIdAndIndex_.begin(),
                channelIdAndIndex_.end(),
                dest.id, &compareIdAndIndexWithId
            );
            if(itDest != channelIdAndIndex_.end())
            {
                auto destNode = inputDevices_[itDest->index].second;
                return DisconnectTask(*this, destNode->inEdges().front());
            }
        }
    }
    else if(source.type == Position::Type::RegularChannelOutput)
    {
        auto itSrc = std::lower_bound(
            channelIdAndIndex_.begin(), channelIdAndIndex_.end(),
            source.id, &compareIdAndIndexWithId
        );
        assert(itSrc != channelIdAndIndex_.end());
        auto srcNode = meters_[itSrc->index].second;
        return DisconnectTask(*this, srcNode->outEdges().front());
    }
    return std::nullopt;
}
}
