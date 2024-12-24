#ifndef YADAW_SRC_CONTROLLER_PLUGINLATENCYUPDATEDCALLBACK
#define YADAW_SRC_CONTROLLER_PLUGINLATENCYUPDATEDCALLBACK

#include "audio/plugin/IAudioPlugin.hpp"
#include "util/Concepts.hpp"

namespace YADAW::Controller
{
namespace Impl
{
void latencyUpdated(YADAW::Audio::Plugin::IAudioPlugin& plugin);
}

template<DerivedTo<YADAW::Audio::Plugin::IAudioPlugin> Plugin>
void latencyUpdated(Plugin& plugin)
{
    Impl::latencyUpdated(
        static_cast<YADAW::Audio::Plugin::IAudioPlugin&>(plugin)
    );
}
}

#endif // YADAW_SRC_CONTROLLER_PLUGINLATENCYUPDATEDCALLBACK
