#ifndef YADAW_SRC_NATIVE_LINUX_MIDIOUTPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_LINUX_MIDIOUTPUTDEVICEIMPL

#if __linux__

#include "midi/MIDIOutputDevice.hpp"

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

#endif //YADAW_SRC_NATIVE_LINUX_MIDIOUTPUTDEVICEIMPL
