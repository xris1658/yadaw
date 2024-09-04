#ifndef YADAW_SRC_NATIVE_MAC_MIDIOUTPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_MAC_MIDIOUTPUTDEVICEIMPL

#if __APPLE__

#include "midi/DeviceInfo.hpp"
#include "midi/MIDIOutputDevice.hpp"

#include <../../System/Library/Frameworks/CoreMIDI.framework/Headers/CoreMIDI.h>

namespace YADAW::MIDI
{
class MIDIOutputDevice::Impl
{
public:
    static std::size_t outputDeviceCount();
    static std::optional<DeviceInfo> outputDeviceAt(std::size_t index);
public:
    Impl(const MIDIOutputDevice& device, const YADAW::Native::MIDIDeviceID& id);
    ~Impl();
public:
    void start(SendToOutputFunc* const func);
    void stop();
private:
};
}

#endif

#endif // YADAW_SRC_NATIVE_MAC_MIDIOUTPUTDEVICEIMPL