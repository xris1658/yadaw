#if(__linux__)

#include "native/VST3Native.hpp"

namespace YADAW::Native
{
using YADAW::Native::Library;
using YADAW::Audio::Plugin::VST3Plugin;

const Steinberg::FIDString ViewType = Steinberg::kPlatformTypeX11EmbedWindowID;

const char* initEntryName = "ModuleEntry";
const char* exitEntryName = "ModuleExit";
}

#endif