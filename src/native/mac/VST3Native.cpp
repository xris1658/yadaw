#if __APPLE__

#include "native/VST3Native.hpp"

#include <pluginterfaces/gui/iplugview.h>

namespace YADAW::Native
{
const Steinberg::FIDString ViewType = Steinberg::kPlatformTypeNSView;

bool initVST3Entry(VST3InitEntry initEntry, void* libraryHandle)
{
    return initEntry && initEntry(libraryHandle);
}
}

#endif