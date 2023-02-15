#ifndef YADAW_SRC_NATIVE_MIDIINPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_MIDIINPUTDEVICEIMPL

#include "midi/MIDIInputDevice.hpp"

#include <winrt/Windows.Devices.Midi.h>

namespace YADAW::MIDI
{
using winrt::Windows::Devices::Midi::MidiInPort;
class MIDIInputDevice::Impl
{
public:
    static std::vector<MIDIInputDeviceInfo> enumerateDevices();
public:
    Impl(const QString& id);
    Impl(const Impl&) = delete;
    Impl(Impl&& rhs) = delete;
    ~Impl();
private:
    MidiInPort midiInPort_;
};
}

#endif //YADAW_SRC_NATIVE_MIDIINPUTDEVICEIMPL
