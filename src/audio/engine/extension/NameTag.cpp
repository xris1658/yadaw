#include "NameTag.hpp"

namespace YADAW::Audio::Engine::Extension
{
NameTag::NameTag(AudioDeviceGraphBase& graph):
    graph_(graph)
{}

void NameTag::onNodeAdded(const ade::NodeHandle& nodeHandle)
{}

void NameTag::onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle)
{}

void NameTag::onConnected(const ade::EdgeHandle& edgeHandle)
{}

void NameTag::onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle)
{}

const NameTag::NodeData& NameTag::getNodeData(const ade::NodeHandle& nodeHandle) const
{
    return getNodeData_(graph_, nodeHandle);
}

NameTag::NodeData& NameTag::getNodeData(const ade::NodeHandle& nodeHandle)
{
    return getNodeData_(graph_, nodeHandle);
}
}
