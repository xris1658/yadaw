#ifndef YADAW_SRC_CONTROLLER_PLUGINLATENCYUPDATEDCALLBACK
#define YADAW_SRC_CONTROLLER_PLUGINLATENCYUPDATEDCALLBACK

#include "audio/plugin/IAudioPlugin.hpp"
#if __cplusplus >= 202002L
#include "util/Concepts.hpp"
#else
#include <type_traits>
#endif

namespace YADAW::Controller
{
namespace Impl
{
void latencyUpdated(YADAW::Audio::Plugin::IAudioPlugin& plugin);
}

#if __cplusplus >=202002L
template<DerivedTo<YADAW::Audio::Plugin::IAudioPlugin> Plugin>
void latencyUpdated(Plugin& plugin)
{
    Impl::latencyUpdated(
        static_cast<YADAW::Audio::Plugin::IAudioPlugin&>(plugin)
    );
}
#else
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
#endif
}

#endif // YADAW_SRC_CONTROLLER_PLUGINLATENCYUPDATEDCALLBACK
