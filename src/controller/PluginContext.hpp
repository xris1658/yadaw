#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTEXT
#define YADAW_SRC_CONTROLLER_PLUGINCONTEXT

#include "audio/engine/AudioDeviceProcess.hpp"
#include "controller/LibraryPluginPool.hpp"
#include "controller/PluginPositionMap.hpp"
#include "util/PolymorphicDeleter.hpp"

#include <set>

class QWindow;

namespace YADAW::Audio::Plugin
{
class CLAPPlugin;
}

namespace YADAW::Controller
{
struct PluginContext
{
    LibraryPluginPool::Instance pluginInstance;
    YADAW::Audio::Engine::AudioDeviceProcess process;
    YADAW::Util::PMRUniquePtr<void> hostContext;
    QWindow* editor = nullptr;
    QWindow* genericEditor = nullptr;
    PluginPosition position;
    YADAW::Util::PMRUniquePtr<void> userData;
};

namespace Impl
{
struct ComparePluginContext
{
    using is_transparent = void;
    bool operator()(
        const PluginContext& lhs,
        const PluginContext& rhs
    ) const
    {
        auto lp = lhs.pluginInstance.plugin();
        YADAW::Audio::Plugin::IAudioPlugin* lptr = lp.has_value()? &(lp->get()): nullptr;
        auto rp = rhs.pluginInstance.plugin();
        YADAW::Audio::Plugin::IAudioPlugin* rptr = rp.has_value()? &(rp->get()): nullptr;
        return lptr < rptr;
    }
    bool operator()(
        const PluginContext& lhs,
        YADAW::Audio::Plugin::IAudioPlugin* rhs
    ) const
    {
        auto lp = lhs.pluginInstance.plugin();
        YADAW::Audio::Plugin::IAudioPlugin* lptr = lp.has_value()? &(lp->get()): nullptr;
        return lptr < rhs;
    }
    bool operator()(
        YADAW::Audio::Plugin::IAudioPlugin* lhs,
        const PluginContext& rhs
    ) const
    {
        auto rp = rhs.pluginInstance.plugin();
        YADAW::Audio::Plugin::IAudioPlugin* rptr = rp.has_value()? &(rp->get()): nullptr;
        return lhs < rptr;
    }
};
}

using PluginContexts = std::set<PluginContext, Impl::ComparePluginContext>;

PluginContexts& appPluginContexts();
}

#endif // YADAW_SRC_CONTROLLER_PLUGINCONTEXT