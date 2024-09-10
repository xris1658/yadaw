#ifndef YADAW_SRC_AUDIO_PLUGIN_PLUGINFORMATSUPPORT
#define YADAW_SRC_AUDIO_PLUGIN_PLUGINFORMATSUPPORT

#include "audio/plugin/AudioPluginBase.hpp"
#include "native/PluginFormatSupport.hpp"

namespace YADAW::Audio::Plugin
{
template<PluginFormat F>
constexpr bool isFormatSupported = YADAW::Native::isPluginFormatSupported<F>;
}

#endif // YADAW_SRC_AUDIO_PLUGIN_PLUGINFORMATSUPPORT