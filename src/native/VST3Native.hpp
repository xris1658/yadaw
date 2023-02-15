#ifndef YADAW_SRC_NATIVE_VST3NATIVE
#define YADAW_SRC_NATIVE_VST3NATIVE

#include "audio/plugin/VST3Plugin.hpp"
#include "native/Library.hpp"

#include <memory>

namespace YADAW::Native
{
extern const Steinberg::FIDString ViewType;

extern const char* initEntryName;
extern const char* exitEntryName;
extern const char* factoryEntryName;

std::shared_ptr<YADAW::Audio::Plugin::VST3Plugin> createVST3FromLibrary(const QString& path);
}

#endif //YADAW_SRC_NATIVE_VST3NATIVE
