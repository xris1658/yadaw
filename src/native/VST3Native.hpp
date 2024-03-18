#ifndef YADAW_SRC_NATIVE_VST3NATIVE
#define YADAW_SRC_NATIVE_VST3NATIVE

#include "native/Library.hpp"

#include <pluginterfaces/base/ftypes.h>

namespace YADAW::Native
{
extern const Steinberg::FIDString ViewType;
#if(WIN32)
using InitEntry = bool(*)();
constexpr char vst3FilePattern[] = "*.vst3";
constexpr char initEntryName[] = "InitDll";
constexpr char exitEntryName[] = "ExitDll";
bool initVST3Entry(InitEntry initEntry, void* libraryHandle = nullptr);
#elif(TARGET_OS_MAC)
using InitEntry = bool(*)(void*);
constexpr char vst3FilePattern[] = "*.so"; // *.dylib?
constexpr char initEntryName[] = "BundleEntry";
constexpr char exitEntryName[] = "BundleExit";
bool initVST3Entry(InitEntry initEntry, void* libraryHandle);
#elif(__linux__)
using InitEntry = bool(*)(void*);
constexpr char vst3FilePattern[] = "*.so";
constexpr char initEntryName[] = "ModuleEntry";
constexpr char exitEntryName[] = "ModuleExit";
bool initVST3Entry(InitEntry initEntry, void* libraryHandle);
#endif
}

#endif // YADAW_SRC_NATIVE_VST3NATIVE
