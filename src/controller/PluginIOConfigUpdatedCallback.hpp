#ifndef YADAW_SRC_CONTROLLER_PLUGINIOCONFIGUPDATEDCALLBACK
#define YADAW_SRC_CONTROLLER_PLUGINIOCONFIGUPDATEDCALLBACK

#include "audio/plugin/IAudioPlugin.hpp"

#include <type_traits>

namespace YADAW::Controller
{
namespace Impl
{
void ioConfigUpdated(YADAW::Audio::Plugin::IAudioPlugin& plugin);
}

template<typename Plugin>
void ioConfigUpdated(Plugin& plugin)
{
    static_assert(
        std::is_base_of_v<
            YADAW::Audio::Plugin::IAudioPlugin,
            Plugin
        >
    );
    Impl::ioConfigUpdated(
        static_cast<YADAW::Audio::Plugin::IAudioPlugin&>(plugin)
    );

}
}

#endif // YADAW_SRC_CONTROLLER_PLUGINIOCONFIGUPDATEDCALLBACK
