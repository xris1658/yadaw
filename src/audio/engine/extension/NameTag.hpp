#ifndef YADAW_SRC_AUDIO_ENGINE_EXTENSION_NAMETAG
#define YADAW_SRC_AUDIO_ENGINE_EXTENSION_NAMETAG

#include "audio/engine/AudioDeviceGraph.hpp"

namespace YADAW::Audio::Engine::Extension
{
class NameTag
{
public:
    struct DataType
    {
        QString name;
    };
public:
    NameTag(AudioDeviceGraphBase& graph,
        DataType&(*getData)(AudioDeviceGraphBase&, const ade::NodeHandle&));
public:
    void onNodeAdded(const ade::NodeHandle& nodeHandle);
    void onNodeAboutToBeRemoved(const ade::NodeHandle& nodeHandle);
    void onConnected(const ade::EdgeHandle& edgeHandle);
    void onAboutToBeDisconnected(const ade::EdgeHandle& edgeHandle);
public:
    const DataType& getData(const ade::NodeHandle& nodeHandle) const;
    DataType& getData(const ade::NodeHandle& nodeHandle);
private:
    AudioDeviceGraphBase& graph_;
    DataType&(*getData_)(AudioDeviceGraphBase&, const ade::NodeHandle&);
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_EXTENSION_NAMETAG