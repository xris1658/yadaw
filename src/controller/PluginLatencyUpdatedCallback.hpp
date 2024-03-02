#ifndef YADAW_SRC_CONTROLLER_PLUGINLATENCYUPDATEDCALLBACK
#define YADAW_SRC_CONTROLLER_PLUGINLATENCYUPDATEDCALLBACK

#include "audio/plugin/IAudioPlugin.hpp"

#include <type_traits>

namespace YADAW::Controller
{
namespace Impl
{
void latencyUpdated(YADAW::Audio::Plugin::IAudioPlugin& plugin);
}

template<typename Plugin>
void latencyUpdated(Plugin& plugin)
{
    static_assert(
        std::is_base_of_v<
            YADAW::Audio::Plugin::IAudioPlugin,
            Plugin
        >
    );
    Impl::latencyUpdated(
        static_cast<YADAW::Audio::Plugin::IAudioPlugin&>(plugin)
    );
}
}

#endif // YADAW_SRC_CONTROLLER_PLUGINLATENCYUPDATEDCALLBACK
