#ifndef YADAW_SRC_NATIVE_PLUGINFORMATSUPPORT
#define YADAW_SRC_NATIVE_PLUGINFORMATSUPPORT

#include "audio/plugin/AudioPluginBase.hpp"

namespace YADAW::Native
{
template<YADAW::Audio::Plugin::PluginFormat>
constexpr bool isPluginFormatSupported = false;

template<>
constexpr bool isPluginFormatSupported<YADAW::Audio::Plugin::PluginFormat::VST3> = true;

template<>
constexpr bool isPluginFormatSupported<YADAW::Audio::Plugin::PluginFormat::CLAP> = true;

template<>
constexpr bool isPluginFormatSupported<YADAW::Audio::Plugin::PluginFormat::Vestifal> = true;

#if __APPLE__
template<>
constexpr bool isPluginFormatSupported<YADAW::Audio::Plugin::PluginFormat::AudioUnitV3> = true;
#endif
}

#endif // YADAW_SRC_NATIVE_PLUGINFORMATSUPPORT