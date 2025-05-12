#ifndef YADAW_SRC_NATIVE_WIN_MIDIOUTPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_WIN_MIDIOUTPUTDEVICEIMPL

#if _WIN32

#include "midi/DeviceInfo.hpp"
#include "midi/MIDIOutputDevice.hpp"

#include "native/win/winrt/Forward.hpp"

#include <../include/winrt/base.h>
#include <../include/winrt/Windows.Foundation.h>
#include <../include/winrt/Windows.Foundation.Collections.h>
#include <../include/winrt/Windows.Devices.Enumeration.h>
#include <../include/winrt/Windows.Devices.Midi.h>

namespace YADAW::MIDI
{
class MIDIOutputDevice::Impl
{
public:
    static std::size_t outputDeviceCount();
    static std::optional<DeviceInfo> outputDeviceAt(std::size_t index);
};
}

#endif

#endif // YADAW_SRC_NATIVE_WIN_MIDIOUTPUTDEVICEIMPL
