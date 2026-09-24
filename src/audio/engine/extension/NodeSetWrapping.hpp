#ifndef YADAW_SRC_AUDIO_ENGINE_EXTENSION_NODESETWRAPPING
#define YADAW_SRC_AUDIO_ENGINE_EXTENSION_NODESETWRAPPING

#include "audio/engine/AudioDeviceGraph.hpp"
#include "audio/engine/NodeSet.hpp"

#include <vector>

namespace YADAW::Audio::Engine::Extension
{
// 
class NodeSetWrapping
{
public:
    struct NodeData
    {
        std::vector<NodeSet*> wrappings_;
    };
private:
    NodeSetWrapping(AudioDeviceGraphBase& graph);
public:
    template<typename... Extensions>
    NodeSetWrapping(AudioDeviceGraph<Extensions...>& graph):
        NodeSetWrapping(static_cast<AudioDeviceGraphBase&>(graph))
    {
        getNodeData_ = static_cast<decltype(getNodeData_)>(
            AudioDeviceGraph<Extensions...>::template getExtensionNodeData<NodeSetWrapping>
        );
    }
public:
    void onNodeAdded(const ade::NodeHandle& nodeHandle);
    void onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle);
    void onConnected(const ade::EdgeHandle& edgeHandle);
    void onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle);
private:
    AudioDeviceGraphBase& graph_;
    NodeData&(*getNodeData_)(AudioDeviceGraphBase&, const ade::NodeHandle&);
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_EXTENSION_NODESETWRAPPING
