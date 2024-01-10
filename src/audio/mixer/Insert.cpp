#include "Insert.hpp"

#include "audio/util/AudioDeviceUtil.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Mixer
{
Insert::Insert(YADAW::Audio::Engine::AudioDeviceGraphBase& graph,
    ade::NodeHandle inNode, ade::NodeHandle outNode,
    std::uint32_t inChannel, std::uint32_t outChannel):
    graph_(graph),
    inNode_(inNode), outNode_(outNode),
    inChannel_(inChannel), outChannel_(outChannel)
{}

std::uint32_t Insert::insertCount() const noexcept
{
    return nodes_.size();
}

bool Insert::empty() const noexcept
{
    return nodes_.empty();
}

std::optional<ade::NodeHandle> Insert::insertAt(std::uint32_t index) const
{
    if(index < insertCount())
    {
        return {nodes_[index]};
    }
    return std::nullopt;
}

bool Insert::insert(const ade::NodeHandle& nodeHandle, std::uint32_t position)
{
    auto fromNode = position == 0? inNode_: nodes_[position - 1];
    auto fromOutEdges = fromNode->outEdges();
    auto fromDevice = graph_.getNodeData(fromNode).process.device();
    auto newDevice = graph_.getNodeData(nodeHandle).process.device();
    auto prevFromChannel = position == 0? inChannel_:
        YADAW::Audio::Util::firstMainAudioOutput(fromDevice);
    auto prevToChannel = YADAW::Audio::Util::firstMainAudioInput(newDevice);
    auto nextFromChannel = YADAW::Audio::Util::firstMainAudioOutput(newDevice);
    auto toNode = position == insertCount()? outNode_: nodes_[position];
    auto toDevice = graph_.getNodeData(toNode).process.device();
    auto nextToChannel = position == insertCount()? outChannel_:
                         YADAW::Audio::Util::firstMainAudioInput(toDevice);
    auto edgeToRemove = *std::find_if(
        fromOutEdges.begin(), fromOutEdges.end(),
        [this, prevFromChannel, nextToChannel](const ade::EdgeHandle& edgeHandle)
        {
            const auto& edgeData = graph_.getEdgeData(edgeHandle);
            return edgeData.fromChannel == prevFromChannel
                && edgeData.toChannel == nextToChannel;
        }
    );
    graph_.disconnect(edgeToRemove);
    graph_.connect(fromNode, nodeHandle, prevFromChannel, prevToChannel);
    graph_.connect(nodeHandle, toNode, nextFromChannel, nextToChannel);
    // Update process sequence
    return true;
}

bool Insert::append(const ade::NodeHandle& nodeHandle)
{
    return insert(nodeHandle, insertCount());
}

bool Insert::remove(std::uint32_t position, std::uint32_t removeCount)
{
    if(removeCount > 0 && position < insertCount() && position + removeCount <= insertCount())
    {
        auto prevNode = position == 0? inNode_: nodes_[position - 1];
        auto nextNode = position + removeCount == insertCount()? outNode_: nodes_[position + removeCount];
        auto fromChannel = position == 0?
            inChannel_:
            YADAW::Audio::Util::firstMainAudioOutput(
                graph_.getNodeData(prevNode).process.device()
            );
        auto toChannel = position + removeCount == insertCount()?
            outChannel_:
            YADAW::Audio::Util::firstMainAudioInput(
                graph_.getNodeData(nextNode).process.device()
            );
        auto itBegin = nodes_.begin() + position;
        auto itEnd = itBegin + removeCount;
        {
            auto node = *itBegin;
            auto inEdgeCount = node->inEdges().size();
            FOR_RANGE0(i, inEdgeCount)
            {
                graph_.disconnect(node->inEdges().front());
            }
        }
        for(auto it = itBegin; it != itEnd; ++it)
        {
            auto node = *it;
            auto outEdgeCount = node->outEdges().size();
            FOR_RANGE0(i, outEdgeCount)
            {
                graph_.disconnect(node->outEdges().front());
            }
        }
        graph_.connect(prevNode, nextNode, fromChannel, toChannel);
        // Remove nodes
        return true;
    }
    return false;
}

void Insert::clear()
{
    remove(0, insertCount());
}
}