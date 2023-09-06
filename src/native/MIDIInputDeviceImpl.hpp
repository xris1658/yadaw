#ifndef YADAW_SRC_NATIVE_MIDIINPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_MIDIINPUTDEVICEIMPL

#if _WIN32
#include "native/win/MIDIInputDeviceImpl.hpp"
#elif __linux__
#include "native/linux/MIDIInputDeviceImpl.hpp"
#endif

#endif // YADAW_SRC_NATIVE_MIDIINPUTDEVICEIMPL
