#ifndef YADAW_SRC_NATIVE_MIDIOUTPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_MIDIOUTPUTDEVICEIMPL

#if _WIN32
#include "native/win/MIDIOutputDeviceImpl.hpp"
#elif __APPLE__
#include "native/mac/MIDIOutputDeviceImpl.hpp"
#elif __linux__
#include "native/linux/MIDIOutputDeviceImpl.hpp"
#endif

#endif //YADAW_SRC_NATIVE_MIDIOUTPUTDEVICEIMPL
