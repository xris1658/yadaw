#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
#define YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH

#include "audio/engine/AudioDeviceProcess.hpp"

#include <ade/node.hpp>
#include <ade/edge.hpp>
#include <ade/graph.hpp>
#include <ade/typed_graph.hpp>
#include <ade/typed_metadata.hpp>

#include <algorithm>
#include <optional>

namespace YADAW::Audio::Engine
{
class AudioDeviceGraph
{
private:
    struct AudioDeviceProcessNode
    {
        AudioDeviceProcess process;
        AudioProcessData<float> processData;
        static const char* name() { return "AudioDeviceProcessNode"; }
    };
public:
    AudioDeviceGraph();
    ade::NodeHandle addNode(AudioDeviceProcess&& process, AudioProcessData<float>&& audioProcessData);
    void removeNode(ade::NodeHandle nodeHandle);
private:
    ade::Graph graph_;
    ade::TypedGraph<AudioDeviceProcessNode> typedGraph_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIODEVICEGRAPH
