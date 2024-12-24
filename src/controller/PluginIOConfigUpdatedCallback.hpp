#ifndef YADAW_SRC_CONTROLLER_PLUGINIOCONFIGUPDATEDCALLBACK
#define YADAW_SRC_CONTROLLER_PLUGINIOCONFIGUPDATEDCALLBACK

#include "audio/plugin/IAudioPlugin.hpp"
#include "util/Concepts.hpp"

namespace YADAW::Controller
{
namespace Impl
{
void ioConfigUpdated(YADAW::Audio::Plugin::IAudioPlugin& plugin);
}

template<DerivedTo<YADAW::Audio::Plugin::IAudioPlugin> Plugin>
void ioConfigUpdated(Plugin& plugin)
{
    Impl::ioConfigUpdated(
        static_cast<YADAW::Audio::Plugin::IAudioPlugin&>(plugin)
    );
}
}

#endif // YADAW_SRC_CONTROLLER_PLUGINIOCONFIGUPDATEDCALLBACK
