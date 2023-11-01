#include "UpstreamLatency.hpp"

namespace YADAW::Audio::Engine::Extension
{
UpstreamLatency::UpstreamLatency(AudioDeviceGraphBase& graph,
    UpstreamLatency::DataType& (* getData)(AudioDeviceGraphBase&, const ade::NodeHandle&)):
    graph_(graph),
    getData_(getData)
{}

std::uint32_t UpstreamLatency::sumLatency(const ade::NodeHandle& nodeHandle)
{
    return getData_(graph_, nodeHandle).upstreamLatency
           + graph_.getNodeData(nodeHandle).process.device()->latencyInSamples();
}

void UpstreamLatency::onNodeAdded(const ade::NodeHandle& nodeHandle)
{
    getData_(graph_, nodeHandle).upstreamLatency = 0U;
}

void UpstreamLatency::onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle)
{}

void UpstreamLatency::onConnected(const ade::EdgeHandle& edgeHandle)
{
    auto toNode = edgeHandle->dstNode();
    auto& upstreamLatency = getData_(graph_, toNode).upstreamLatency;
    auto it = std::max_element(toNode->inNodes().begin(), toNode->inNodes().end(),
        [this](const ade::NodeHandle& lhs, const ade::NodeHandle& rhs)
        {
            return sumLatency(lhs) < sumLatency(rhs);
        }
    );
    upstreamLatency = sumLatency(*it);
}

void UpstreamLatency::onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle)
{
    const auto& fromNode = edgeHandle->srcNode();
    auto toNode = edgeHandle->dstNode();
    auto& upstreamLatency = getData_(graph_, toNode).upstreamLatency;
    auto newUpstreamLatency = 0U;
    for(const auto& nodeHandle: toNode->inNodes())
    {
        if(nodeHandle != fromNode)
        {
            if(auto sumLatency = this->sumLatency(nodeHandle);
                newUpstreamLatency < sumLatency)
            {
                newUpstreamLatency = sumLatency;
            }
        }
    }
    upstreamLatency = newUpstreamLatency;
}

std::uint32_t UpstreamLatency::getUpstreamLatency(const ade::NodeHandle& nodeHandle)
{
    return getData_(graph_, nodeHandle).upstreamLatency;
}
}