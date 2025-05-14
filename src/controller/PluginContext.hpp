#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTEXT
#define YADAW_SRC_CONTROLLER_PLUGINCONTEXT

#include "audio/engine/AudioDeviceProcess.hpp"
#include "controller/LibraryPluginPool.hpp"
#include "controller/PluginPositionMap.hpp"
#include "util/PolymorphicDeleter.hpp"

#include <set>

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

namespace Impl
{
class ComparePluginContext
{
    using is_transparent = void;
    bool operator()(
        const LibraryPluginPool::Instance& lhs,
        const LibraryPluginPool::Instance& rhs
    )
    {
        auto lp = lhs.plugin();
        YADAW::Audio::Plugin::IAudioPlugin* lptr = lp.has_value()? &(lp->get()): nullptr;
        auto rp = rhs.plugin();
        YADAW::Audio::Plugin::IAudioPlugin* rptr = rp.has_value()? &(rp->get()): nullptr;
        return lptr < rptr;
    }
    bool operator()(
        const LibraryPluginPool::Instance& lhs,
        YADAW::Audio::Plugin::IAudioPlugin* rhs
    )
    {
        auto lp = lhs.plugin();
        YADAW::Audio::Plugin::IAudioPlugin* lptr = lp.has_value()? &(lp->get()): nullptr;
        return lptr < rhs;
    }
    bool operator()(
        YADAW::Audio::Plugin::IAudioPlugin* lhs,
        const LibraryPluginPool::Instance& rhs
    )
    {
        auto rp = rhs.plugin();
        YADAW::Audio::Plugin::IAudioPlugin* rptr = rp.has_value()? &(rp->get()): nullptr;
        return lhs < rptr;
    }
};
}

using PluginContexts = std::set<PluginContext, Impl::ComparePluginContext>;

PluginContexts& appPluginContexts();
}

#endif // YADAW_SRC_CONTROLLER_PLUGINCONTEXT