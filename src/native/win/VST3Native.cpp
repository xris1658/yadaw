#if(WIn32)

#include "native/VST3Native.hpp"

#include "native/Library.hpp"

namespace YADAW::Native
{
using YADAW::Native::Library;
using YADAW::Audio::Plugin::VST3Plugin;

const Steinberg::FIDString ViewType = Steinberg::kPlatformTypeHWND;

const char* initEntryName = "InitDll";
const char* exitEntryName = "ExitDll";

}

#endif