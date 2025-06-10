#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTEXT
#define YADAW_SRC_CONTROLLER_PLUGINCONTEXT

#include "audio/engine/AudioDeviceProcess.hpp"
#include "audio/engine/AudioDeviceGraphBase.hpp"
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
    YADAW::Util::PMRUniquePtr<void> hostContext;
    LibraryPluginPool::Instance pluginInstance;
    YADAW::Audio::Engine::AudioDeviceProcess process;
    QWindow* editor = nullptr;
    QWindow* genericEditor = nullptr;
    PluginPosition position;
    ade::NodeHandle node;
    YADAW::Util::PMRUniquePtr<void> userData;
};

struct InitPluginArgs
{
    std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t> mainInputChannelGroup;
    std::pair<YADAW::Audio::Base::ChannelGroupType, std::uint32_t> mainOutputChannelGroup;
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