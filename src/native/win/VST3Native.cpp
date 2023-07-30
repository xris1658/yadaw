#if(WIN32)

#include "native/VST3Native.hpp"

namespace YADAW::Native
{
const Steinberg::FIDString ViewType = Steinberg::kPlatformHWND;

bool initVST3Entry(InitEntry initEntry)
{
    if(initEntry)
    {
        return initEntry();
    }
    return true;
}
}

#endif