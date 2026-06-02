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
    struct DataType
    {
        // TODO: Add a tree-like structure here... maybe use `TreeNode`?
    };
public:
    NodeSetWrapping(AudioDeviceGraphBase& graph,
    DataType&(*getData)(AudioDeviceGraphBase&, const ade::NodeHandle&));
public:

public:
    void onNodeAdded(const ade::NodeHandle& nodeHandle);
    void onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle);
    void onConnected(const ade::EdgeHandle& edgeHandle);
    void onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle);
private:
    AudioDeviceGraphBase& graph_;
    DataType&(*getData_)(AudioDeviceGraphBase&, const ade::NodeHandle&);
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_EXTENSION_NODESETWRAPPING
