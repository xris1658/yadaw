#ifndef YADAW_SRC_AUDIO_ENGINE_NODESET
#define YADAW_SRC_AUDIO_ENGINE_NODESET

#include "audio/engine/AudioDeviceGraphBase.hpp"

#include <ade/node.hpp>
#include <ade/handle.hpp>

namespace YADAW::Audio::Engine
{
// `NodeSet` manages part of an `AudioDeviceGraphBase` and provide extra
// convenient functions to access and modify the portion so that we don't need
// to subclass `IAudioDevice` and manually maintain a graph-like mechanism
// inside it.
// I initially created this class to introduce `YADAW::Audio::Mixer::MiniMix`,
// but this might solve more problems than I expect:
// - Plugins that use multichannel speaker arrangements might have different
//   channel definitions depending on the plugin API, and some plugins don't
//   support multichannel speaker arrangements at all and make every channel an
//   mono channel group. We can sandwich the plugin node with two adaptors (like
//   `ChannelGroupRemixer`) and manage those nodes into a `NodeSet`.
// - PDC is implemented by `MultiInputDeviceWithPDC` that maintains a simple
//   graph in itself, being a perfect facility to be renovated into a `NodeSet`.
//   The only problem is that we have to manually create and destroy the PDC
//   device, which is an error-prone way (unless we use a wrapper of the graph
//   like `AudioDeviceGraphWithPDC`, also error-prone in its own right).
// `NodeSet` can be nested, but not intersected.
class NodeSet // TODO
{
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_NODESET
