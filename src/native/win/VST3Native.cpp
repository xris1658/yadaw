#if _WIN32

#include "native/VST3Native.hpp"

#include <pluginterfaces/gui/iplugview.h>

namespace YADAW::Native
{
const Steinberg::FIDString ViewType = Steinberg::kPlatformTypeHWND;

bool initVST3Entry(VST3InitEntry initEntry, void*)
{
    if(initEntry)
    {
        return initEntry();
    }
    return true;
}
}

#endif