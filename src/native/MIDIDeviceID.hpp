#ifndef YADAW_SRC_NATIVE_MIDIDEVICEID
#define YADAW_SRC_NATIVE_MIDIDEVICEID

#if(WIN32)
#include <winrt/base.h>
#elif(__linux__)
#endif

namespace YADAW::Native
{
#if(WIN32)
using MIDIDeviceID = winrt::hstring;
#elif(__linux__)
using MIDIDeviceID = int;
#endif
}

#endif //YADAW_SRC_NATIVE_MIDIDEVICEID
