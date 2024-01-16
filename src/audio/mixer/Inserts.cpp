#include "Inserts.hpp"

#include "audio/util/AudioDeviceUtil.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Mixer
{
Inserts::Inserts(YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
    ade::NodeHandle inNode, ade::NodeHandle outNode,
    std::uint32_t inChannel, std::uint32_t outChannel):
    graph_(graph),
    inNode_(std::move(inNode)), outNode_(std::move(outNode)),
    inChannel_(inChannel), outChannel_(outChannel)
{
    graph_.connect(inNode_, outNode_, inChannel_, outChannel_);
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
        channel_.insert(channel_.begin() + position,
            std::make_pair(inChannel, outChannel)
        );
        setBypassed(position, false);
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
        nodes_.erase(nodes_.begin() + position,
            nodes_.begin() + position + removeCount);
        names_.erase(names_.begin() + position,
            names_.begin() + position + removeCount);
        bypassed_.erase(bypassed_.begin() + position,
            bypassed_.begin() + position + removeCount);
        channel_.erase(channel_.begin() + position,
            channel_.begin() + position + removeCount);
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
        std::uint32_t prevChannel = inChannel_;
        std::uint32_t nextChannel = outChannel_;
        for(auto i = position; i-- > 0; --i)
        {
            if(!bypassed_[i])
            {
                prevNode = nodes_[i];
                prevChannel = channel_[i].second;
            }
        }
        for(auto i = position; i < insertCount; ++i)
        {
            if(!bypassed_[i])
            {
                nextNode = nodes_[i];
                nextChannel = channel_[i].first;
            }
        }
        if(bypassed)
        {
            for(const auto& edgeHandle: prevNode->outEdges())
            {
                const auto& edgeData = graph_.getEdgeData(edgeHandle);
                if(edgeHandle->dstNode() == nodes_[position]
                   && edgeData.fromChannel == prevChannel
                   && edgeData.toChannel == channel_[position].first)
                {
                    graph_.disconnect(edgeHandle);
                    break;
                }
            }
            for(const auto& edgeHandle: nextNode->inEdges())
            {
                const auto& edgeData = graph_.getEdgeData(edgeHandle);
                if(edgeHandle->srcNode() == nodes_[position]
                    && edgeData.fromChannel == channel_[position].second
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
                    graph_.connect(prevNode, nodes_[position],
                        prevChannel, channel_[position].first);
                    graph_.connect(nodes_[position], nextNode,
                        channel_[position].second, nextChannel);
                    break;
                }
            }
        }
        bypassed_[position] = bypassed;
    }
}
}