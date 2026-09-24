#ifndef YADAW_SRC_AUDIO_ENGINE_EXTENSION_NAMETAG
#define YADAW_SRC_AUDIO_ENGINE_EXTENSION_NAMETAG

#include "audio/engine/AudioDeviceGraph.hpp"

namespace YADAW::Audio::Engine::Extension
{
class NameTag
{
public:
    struct GraphData
    {
        QString name; // TODO
    };
    struct NodeData
    {
        QString name;
    };
    struct EdgeData
    {
        QString name;
    };
private:
    NameTag(AudioDeviceGraphBase& graph);
public:
    template<typename... Extensions>
    NameTag(AudioDeviceGraph<Extensions...>& graph):
        NameTag(static_cast<AudioDeviceGraphBase&>(graph))
    {
        getNodeData_ = static_cast<decltype(getNodeData_)>(
            AudioDeviceGraph<Extensions...>::template getExtensionNodeData<NameTag>
        );
        getEdgeData_ = static_cast<decltype(getEdgeData_)>(
            AudioDeviceGraph<Extensions...>::template getExtensionEdgeData<NameTag>
        );
        getGraphData_ = static_cast<decltype(getGraphData_)>(
            AudioDeviceGraph<Extensions...>::template getExtensionGraphData<NameTag>
        );
    }
public:
    void onNodeAdded(const ade::NodeHandle& nodeHandle);
    void onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle);
    void onConnected(const ade::EdgeHandle& edgeHandle);
    void onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle);
public:
    const NodeData& getNodeData(const ade::NodeHandle& nodeHandle) const;
    NodeData& getNodeData(const ade::NodeHandle& nodeHandle);
private:
    AudioDeviceGraphBase& graph_;
    NodeData&(*getNodeData_)(AudioDeviceGraphBase&, const ade::NodeHandle&);
    EdgeData&(*getEdgeData_)(AudioDeviceGraphBase&, const ade::EdgeHandle&);
    GraphData&(*getGraphData_)(AudioDeviceGraphBase&);
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_EXTENSION_NAMETAG