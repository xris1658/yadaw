#include "UpstreamLatency.hpp"

#include "util/ADEUtil.hpp"
#include "util/IntegerRange.hpp"

#include <set>
#include <queue>
#include <utility>

namespace YADAW::Audio::Engine::Extension
{
void blankCallback(const UpstreamLatency& sender, const ade::NodeHandle& nodeHandle)
{}

UpstreamLatency::UpstreamLatency(AudioDeviceGraphBase& graph,
    UpstreamLatency::DataType& (* getData)(AudioDeviceGraphBase&, const ade::NodeHandle&)):
    graph_(graph),
    getData_(getData),
    callback_(&blankCallback)
{}

std::uint32_t UpstreamLatency::sumLatency(const ade::NodeHandle& nodeHandle) const
{
    return getMaxUpstreamLatency(nodeHandle)
        + graph_.getNodeData(nodeHandle).process.device()->latencyInSamples();
}

void UpstreamLatency::onNodeAdded(const ade::NodeHandle& nodeHandle)
{
    getData_(graph_, nodeHandle).upstreamLatency = 0U;
    auto& upstreamLatencies = getData_(graph_, nodeHandle).upstreamLatencies;
    auto device = graph_.getNodeData(nodeHandle).process.device();
    auto inputCount = device->audioInputGroupCount();
    upstreamLatencies.resize(inputCount);
    std::fill(upstreamLatencies.begin(), upstreamLatencies.end(), 0U);
}

void UpstreamLatency::onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle)
{}

void UpstreamLatency::onConnected(const ade::EdgeHandle& edgeHandle)
{
    auto toChannel = graph_.getEdgeData(edgeHandle).toChannel;
    updateUpstreamLatency(edgeHandle->dstNode(), {edgeHandle}, graph_.getEdgeData(edgeHandle).toChannel);
}

void UpstreamLatency::onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle)
{
    auto toChannel = graph_.getEdgeData(edgeHandle).toChannel;
    auto& upstreamLatencies = getData_(graph_, edgeHandle->dstNode()).upstreamLatencies;
    updateUpstreamLatency(edgeHandle->dstNode(), std::nullopt, graph_.getEdgeData(edgeHandle).toChannel);
}

std::optional<std::uint32_t> UpstreamLatency::getUpstreamLatency(
    const ade::NodeHandle& nodeHandle, std::uint32_t audioInputGroupIndex) const
{
    const auto& upstreamLatencies = getData_(graph_, nodeHandle).upstreamLatencies;
    if(audioInputGroupIndex < upstreamLatencies.size())
    {
        return {upstreamLatencies[audioInputGroupIndex]};
    }
    return std::nullopt;
}

std::uint32_t UpstreamLatency::getMaxUpstreamLatency(
    const ade::NodeHandle& nodeHandle) const
{
    const auto& upstreamLatencies = getData_(graph_, nodeHandle).upstreamLatencies;
    auto it = std::max_element(upstreamLatencies.begin(), upstreamLatencies.end());
    if(it == upstreamLatencies.end())
    {
        return 0U;
    }
    return *it;
}

void UpstreamLatency::setLatencyOfNodeUpdatedCallback(
    std::function<LatencyOfNodeUpdatedCallback> function)
{
    callback_ = std::move(function);
}

void UpstreamLatency::resetLatencyOfNodeUpdatedCallback()
{
    callback_ = &blankCallback;
}

void UpstreamLatency::updateUpstreamLatency(
    const ade::NodeHandle& outNode,
    std::optional<ade::EdgeHandle> edge,
    std::uint32_t audioInputGroupIndex)
{
    struct OutNodeElement
    {
        ade::NodeHandle outNode;
        std::uint32_t audioInputGroupIndex;
        std::uint32_t latency;
        OutNodeElement(const ade::NodeHandle& outNode,
            std::uint32_t audioInputGroupIndex, std::uint32_t latency):
            outNode(outNode), audioInputGroupIndex(audioInputGroupIndex), latency(latency) {}
    };
    std::queue<OutNodeElement> queue;
    std::set<ade::NodeHandle, YADAW::Util::CompareNodeHandle> latencyUpdatedNodes;
    auto latency = edge.has_value()? sumLatency((*edge)->srcNode()): 0U;
    queue.emplace(outNode, audioInputGroupIndex, latency);
    do
    {
        FOR_RANGE0(i, queue.size())
        {
            auto& [outNode, audioInputGroupIndex, latency] = queue.front();
            auto it = latencyUpdatedNodes.find(outNode);
            if(it == latencyUpdatedNodes.end())
            {
                latencyUpdatedNodes.emplace_hint(it, outNode);
            }
            getData_(graph_, outNode).upstreamLatencies[audioInputGroupIndex] = latency;
            for(const auto& outEdge: outNode->outEdges())
            {
                queue.emplace(outEdge->dstNode(), graph_.getEdgeData(outEdge).toChannel, sumLatency(outNode));
            }
            queue.pop();
        }
    }
    while(!queue.empty());
    for(const auto& node: latencyUpdatedNodes)
    {
        callback_(*this, node);
    }
}
}