#include "Inserts.hpp"

#include "audio/util/AudioDeviceUtil.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Mixer
{
void blankConnectionUpdatedCallback(const Inserts&) {}

Inserts::Inserts(YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
    IDGen& auxInputIDGen, IDGen& auxOutputIDGen,
    ade::NodeHandle inNode, ade::NodeHandle outNode,
    std::uint32_t inChannelGroupIndex, std::uint32_t outChannelGroupIndex):
    graph_(graph),
    inNode_(std::move(inNode)), outNode_(std::move(outNode)),
    inChannelGroupIndex_(inChannelGroupIndex), outChannelGroupIndex_(outChannelGroupIndex),
    auxInputIDGen_(&auxInputIDGen), auxOutputIDGen_(&auxOutputIDGen),
    connectionUpdatedCallback_(blankConnectionUpdatedCallback)
{
    auto inDevice = graph_.getNodeData(inNode_).process.device();
    auto outDevice = graph_.getNodeData(outNode_).process.device();
    if(inChannelGroupIndex < inDevice->audioOutputGroupCount()
       && outChannelGroupIndex < outDevice->audioInputGroupCount()
       && inDevice->audioOutputGroupAt(inChannelGroupIndex)->get().channelCount()
            == outDevice->audioInputGroupAt(outChannelGroupIndex)->get().channelCount())
    {
        graph_.connect(inNode_, outNode_, inChannelGroupIndex_, outChannelGroupIndex_);
    }
    else
    {
        throw std::invalid_argument("");
    }
}

Inserts::~Inserts()
{
    clear();
    for(const auto& edgeHandle: inNode_->outEdges())
    {
        if(auto& edgeData = graph_.getEdgeData(edgeHandle);
            edgeData.fromChannel == inChannelGroupIndex_
            && edgeHandle->dstNode() == outNode_
            && edgeData.toChannel == outChannelGroupIndex_
        )
        {
            graph_.disconnect(edgeHandle);
            break;
        }
    }
}

YADAW::Audio::Engine::AudioDeviceGraphBase& Inserts::graph() const
{
    return graph_;
}

std::uint32_t Inserts::insertCount() const noexcept
{
    return nodes_.size();
}

bool Inserts::empty() const noexcept
{
    return nodes_.empty();
}

std::optional<ade::NodeHandle> Inserts::insertAt(std::uint32_t index) const
{
    if(index < insertCount())
    {
        return {nodes_[index]};
    }
    return std::nullopt;
}

std::optional<QString> Inserts::insertNameAt(std::uint32_t index) const
{
    if(index < insertCount())
    {
        return {names_[index]};
    }
    return std::nullopt;
}

std::optional<std::uint32_t> Inserts::insertLatencyAt(std::uint32_t index) const
{
    if(index < insertCount())
    {
        return graph_.getNodeData(nodes_[index]).process.device()
        ->latencyInSamples();
    }
    return std::nullopt;
}

std::optional<bool> Inserts::insertBypassed(std::uint32_t index) const
{
    if(index < insertCount())
    {
        return {static_cast<bool>(bypassed_[index])};
    }
    return std::nullopt;
}

std::optional<IDGen::ID> Inserts::insertAuxInputID(
    std::uint32_t insertIndex, std::uint32_t channelGroupIndex) const
{
    if(insertIndex < insertCount()
        && channelGroupIndex <= auxInputIDs_[insertIndex].size())
    {
        if(channelGroupIndex > channelGroupIndices_[insertIndex].first)
        {
            return {auxInputIDs_[insertIndex][channelGroupIndex - 1]};
        }
        else if(channelGroupIndex < channelGroupIndices_[insertIndex].first)
        {
            return {auxInputIDs_[insertIndex][channelGroupIndex]};
        }
    }
    return std::nullopt;
}

std::optional<IDGen::ID> Inserts::insertAuxOutputID(
    std::uint32_t insertIndex, std::uint32_t channelGroupIndex) const
{
    if(insertIndex < insertCount()
        && channelGroupIndex <= auxInputIDs_[insertIndex].size())
    {
        if(channelGroupIndex > channelGroupIndices_[insertIndex].second)
        {
            return {auxInputIDs_[insertIndex][channelGroupIndex - 1]};
        }
        else if(channelGroupIndex < channelGroupIndices_[insertIndex].second)
        {
            return {auxInputIDs_[insertIndex][channelGroupIndex]};
        }
    }
    return std::nullopt;
}

const ade::NodeHandle& Inserts::inNode() const
{
    return inNode_;
}

const ade::NodeHandle& Inserts::outNode() const
{
    return outNode_;
}

std::uint32_t Inserts::inChannelGroupIndex() const
{
    return inChannelGroupIndex_;
}

std::uint32_t Inserts::outChannelGroupIndex() const
{
    return outChannelGroupIndex_;
}

bool Inserts::setInNode(const ade::NodeHandle& inNode, std::uint32_t inChannelGroupIndex)
{
    auto inDevice = graph_.getNodeData(inNode).process.device();
    if(inChannelGroupIndex < inDevice->audioOutputGroupCount())
    {
        auto toNode = outNode_;
        auto toChannel = outChannelGroupIndex_;
        FOR_RANGE0(i, insertCount())
        {
            if(!bypassed_[i])
            {
                toNode = nodes_[i];
                toChannel = channelGroupIndices_[i].first;
                break;
            }
        }
        auto outDevice = graph_.getNodeData(toNode).process.device();
        if(inDevice->audioOutputGroupAt(inChannelGroupIndex)->get().channelCount()
            == outDevice->audioInputGroupAt(toChannel)->get().channelCount())
        {
            auto outEdges = inNode_->outEdges();
            auto it = std::find_if(outEdges.begin(), outEdges.end(),
                [this, &toNode, toChannel](const ade::EdgeHandle& edgeHandle)
                {
                    return edgeHandle->dstNode() == toNode
                        && graph_.getEdgeData(edgeHandle).fromChannel == inChannelGroupIndex_
                        && graph_.getEdgeData(edgeHandle).toChannel == toChannel;
                }
            );
            if(it != outEdges.end())
            {
                graph_.disconnect(*it);
            }
            graph_.connect(inNode, toNode, inChannelGroupIndex, toChannel);
            inNode_ = inNode;
            inChannelGroupIndex_ = inChannelGroupIndex;
            connectionUpdatedCallback_(*this);
            return true;
        }
    }
    return false;
}

bool Inserts::setOutNode(const ade::NodeHandle& outNode, std::uint32_t outChannelGroupIndex)
{
    auto outDevice = graph_.getNodeData(outNode).process.device();
    if(outChannelGroupIndex < outDevice->audioInputGroupCount())
    {
        auto fromNode = inNode_;
        auto fromChannel = inChannelGroupIndex_;
        for(auto i = insertCount(); i-- > 0;)
        {
            if(!bypassed_[i])
            {
                fromNode = nodes_[i];
                fromChannel = channelGroupIndices_[i].second;
                break;
            }
        }
        auto inDevice = graph_.getNodeData(fromNode).process.device();
        if(outDevice->audioInputGroupAt(outChannelGroupIndex)->get().channelCount()
            && inDevice->audioOutputGroupAt(fromChannel)->get().channelCount())
        {
            auto inEdges = outNode_->inEdges();
            auto it = std::find_if(inEdges.begin(), inEdges.end(),
                [this, &fromNode, fromChannel](const ade::EdgeHandle& edgeHandle)
                {
                    return edgeHandle->srcNode() == fromNode
                        && graph_.getEdgeData(edgeHandle).fromChannel == fromChannel
                        && graph_.getEdgeData(edgeHandle).toChannel == outChannelGroupIndex_;
                }
            );
            if(it != inEdges.end())
            {
                graph_.disconnect(*it);
            }
            graph_.connect(fromNode, outNode, fromChannel, outChannelGroupIndex);
            outNode_ = outNode;
            outChannelGroupIndex_ = outChannelGroupIndex;
            connectionUpdatedCallback_(*this);
            return true;
        }
    }
    return false;
}

bool Inserts::insert(const ade::NodeHandle& nodeHandle,
    std::uint32_t position, const QString& name)
{
    if(position <= insertCount())
    {
        nodes_.insert(nodes_.begin() + position, nodeHandle);
        names_.insert(names_.begin() + position, name);
        bypassed_.insert(bypassed_.begin() + position, true);
        auto device = graph_.getNodeData(nodeHandle).process.device();
        auto inChannel = device->audioInputGroupCount();
        auto outChannel = device->audioOutputGroupCount();
        FOR_RANGE0(i, inChannel)
        {
            if(device->audioInputGroupAt(i)->get().isMain())
            {
                inChannel = i;
                break;
            }
        }
        FOR_RANGE0(i, outChannel)
        {
            if(device->audioOutputGroupAt(i)->get().isMain())
            {
                outChannel = i;
                break;
            }
        }
        channelGroupIndices_.insert(channelGroupIndices_.begin() + position,
            std::make_pair(inChannel, outChannel)
        );
        setBypassed(position, false);
        auto& inputIDs = *auxInputIDs_.emplace(
            auxInputIDs_.begin() + position
        );
        auto& outputIDs = *auxOutputIDs_.emplace(
            auxOutputIDs_.begin() + position
        );
        inputIDs.reserve(device->audioInputGroupCount() - 1);
        inputIDs.reserve(device->audioOutputGroupCount() - 1);
        std::generate_n(
            std::back_inserter(inputIDs), device->audioInputGroupCount() - 1,
            [this]()
            {
                return (*auxInputIDGen_)();
            }
        );
        std::generate_n(
            std::back_inserter(outputIDs), device->audioOutputGroupCount() - 1,
            [this]()
            {
                return (*auxOutputIDGen_)();
            }
        );
        connectionUpdatedCallback_(*this);
        return true;
    }
    return false;
}

bool Inserts::append(const ade::NodeHandle& nodeHandle, const QString& name)
{
    return insert(nodeHandle, insertCount(), name);
}

bool Inserts::remove(std::uint32_t position, std::uint32_t removeCount)
{
    if(removeCount > 0 && position < insertCount()
        && position + removeCount <= insertCount())
    {
        FOR_RANGE(i, position, position + removeCount)
        {
            setBypassed(i, true);
        }
        auxInputIDs_.erase(
            auxInputIDs_.begin() + position,
            auxInputIDs_.begin() + position + removeCount
        );
        auxOutputIDs_.erase(
            auxOutputIDs_.begin() + position,
            auxOutputIDs_.begin() + position + removeCount
        );
        nodes_.erase(nodes_.begin() + position,
            nodes_.begin() + position + removeCount);
        names_.erase(names_.begin() + position,
            names_.begin() + position + removeCount);
        bypassed_.erase(bypassed_.begin() + position,
            bypassed_.begin() + position + removeCount);
        channelGroupIndices_.erase(channelGroupIndices_.begin() + position,
            channelGroupIndices_.begin() + position + removeCount);
        return true;
    }
    return false;
}

void Inserts::clear()
{
    remove(0, insertCount());
}

void Inserts::setName(std::uint32_t position, const QString& name)
{
    if(position < insertCount())
    {
        names_[position] = name;
    }
}

void Inserts::setBypassed(std::uint32_t position, bool bypassed)
{
    auto insertCount = this->insertCount();
    if(position < insertCount && (bypassed ^ bypassed_[position]))
    {
        ade::NodeHandle prevNode = inNode_;
        ade::NodeHandle nextNode = outNode_;
        std::uint32_t prevChannel = inChannelGroupIndex_;
        std::uint32_t nextChannel = outChannelGroupIndex_;
        for(auto i = position; i-- > 0;)
        {
            if(!bypassed_[i])
            {
                prevNode = nodes_[i];
                prevChannel = channelGroupIndices_[i].second;
                break;
            }
        }
        for(auto i = position + 1; i < insertCount; ++i)
        {
            if(!bypassed_[i])
            {
                nextNode = nodes_[i];
                nextChannel = channelGroupIndices_[i].first;
                break;
            }
        }
        if(bypassed)
        {
            for(const auto& edgeHandle: prevNode->outEdges())
            {
                const auto& edgeData = graph_.getEdgeData(edgeHandle);
                if(edgeHandle->dstNode() == nodes_[position]
                   && edgeData.fromChannel == prevChannel
                   && edgeData.toChannel == channelGroupIndices_[position].first)
                {
                    graph_.disconnect(edgeHandle);
                    break;
                }
            }
            for(const auto& edgeHandle: nextNode->inEdges())
            {
                const auto& edgeData = graph_.getEdgeData(edgeHandle);
                if(edgeHandle->srcNode() == nodes_[position]
                    && edgeData.fromChannel == channelGroupIndices_[position].second
                    && edgeData.toChannel == nextChannel)
                {
                    graph_.disconnect(edgeHandle);
                    break;
                }
            }
            graph_.connect(prevNode, nextNode, prevChannel, nextChannel);
        }
        else
        {
            for(const auto& edgeHandle: prevNode->outEdges())
            {
                const auto& edgeData = graph_.getEdgeData(edgeHandle);
                if(edgeHandle->dstNode() == nextNode
                    && edgeData.fromChannel == prevChannel
                    && edgeData.toChannel == nextChannel)
                {
                    graph_.disconnect(edgeHandle);
                    break;
                }
            }
            graph_.connect(prevNode, nodes_[position],
                prevChannel, channelGroupIndices_[position].first);
            graph_.connect(nodes_[position], nextNode,
                channelGroupIndices_[position].second, nextChannel);
        }
        bypassed_[position] = bypassed;
        connectionUpdatedCallback_(*this);
    }
}

bool Inserts::move(std::uint32_t position, std::uint32_t count,
    Inserts& rhs, std::uint32_t destPosition)
{
    if(count > 0 && position < insertCount()
        && position + count <= insertCount())
    {
        std::vector<bool> oldBypassed(count, false);
        std::copy(bypassed_.begin() + position,
            bypassed_.begin() + position + count,
            oldBypassed.begin());
        if(this == &rhs)
        {
            if(position != destPosition
               && position + count < destPosition)
            {
                std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> rotate =
                    position > destPosition?
                        std::make_tuple(destPosition, position, position + count):
                        std::make_tuple(position, position + count, destPosition);
                const auto& [first, middle, last] = rotate;
                std::rotate(
                    auxInputIDs_.begin() + first,
                    auxInputIDs_.begin() + middle,
                    auxInputIDs_.begin() + last);
                std::rotate(
                    auxOutputIDs_.begin() + first,
                    auxOutputIDs_.begin() + middle,
                    auxOutputIDs_.begin() + last);
                std::rotate(
                    nodes_.begin() + first,
                    nodes_.begin() + middle,
                    nodes_.begin() + last);
                std::rotate(
                    names_.begin() + first,
                    names_.begin() + middle,
                    names_.begin() + last);
                std::rotate(
                    bypassed_.begin() + first,
                    bypassed_.begin() + middle,
                    bypassed_.begin() + last);
                std::rotate(
                    channelGroupIndices_.begin() + first,
                    channelGroupIndices_.begin() + middle,
                    channelGroupIndices_.begin() + last);
                FOR_RANGE0(i, count)
                {
                    setBypassed(destPosition + i, oldBypassed[i]);
                }
                return true;
            }
        }
        else
        {
            FOR_RANGE(i, position, position + count)
            {
                setBypassed(i, true);
            }
            FOR_RANGE0(i, count)
            {
                rhs.insert(nodes_[position + i], destPosition + i, names_[position + i]);
                rhs.setBypassed(destPosition + i, oldBypassed[i]);
            }
            remove(position, count);
            return true;
        }
    }
    return false;
}

void Inserts::setConnectionUpdatedCallback(ConnectionUpdatedCallback* callback)
{
    connectionUpdatedCallback_ = callback;
}
}