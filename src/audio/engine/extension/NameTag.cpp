#include "NameTag.hpp"

namespace YADAW::Audio::Engine::Extension
{
NameTag::NameTag(AudioDeviceGraphBase& graph,
    DataType&(*getData)(AudioDeviceGraphBase&, const ade::NodeHandle&)):
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

const NameTag::DataType& NameTag::getData(const ade::NodeHandle& nodeHandle) const
{
    return getData_(graph_, nodeHandle);
}

NameTag::DataType& NameTag::getData(const ade::NodeHandle& nodeHandle)
{
    return getData_(graph_, nodeHandle);
}
}
