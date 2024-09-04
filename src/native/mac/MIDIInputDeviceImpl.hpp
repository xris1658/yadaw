#ifndef YADAW_SRC_NATIVE_MAC_MIDIINPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_MAC_MIDIINPUTDEVICEIMPL

#if __APPLE__

#include "midi/DeviceInfo.hpp"
#include "midi/MIDIInputDevice.hpp"

#include <../../System/Library/Frameworks/CoreMIDI.framework/Headers/CoreMIDI.h>

namespace YADAW::MIDI
{
class MIDIInputDevice::Impl
{
public:
    static std::size_t inputDeviceCount();
    static std::optional<DeviceInfo> inputDeviceAt(std::size_t index);
public:
    Impl(const MIDIInputDevice& device, const YADAW::Native::MIDIDeviceID& id);
    ~Impl();
public:
    void start(ReceiveInputFunc* const func);
    void stop();
private:
};
}

#endif

#endif // YADAW_SRC_NATIVE_MAC_MIDIINPUTDEVICEIMPL