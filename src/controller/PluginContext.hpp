#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTEXT
#define YADAW_SRC_CONTROLLER_PLUGINCONTEXT

#include "audio/engine/AudioDeviceProcess.hpp"
#include "controller/LibraryPluginPool.hpp"
#include "controller/PluginPositionMap.hpp"
#include "util/PolymorphicDeleter.hpp"

class QWindow;

namespace YADAW::Controller
{
struct PluginContext
{
    LibraryPluginPool::Instance pluginInstance;
    YADAW::Audio::Engine::AudioDeviceProcess process;
    YADAW::Util::PMRUniquePtr<void> hostContext;
    QWindow* editor = nullptr;
    QWindow* genericEditor = nullptr;
    YADAW::Util::PMRUniquePtr<void> userData;
};
}

#endif // YADAW_SRC_CONTROLLER_PLUGINCONTEXT