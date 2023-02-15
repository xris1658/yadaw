#ifndef YADAW_SRC_NATIVE_MIDIOUTPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_MIDIOUTPUTDEVICEIMPL

#include "midi/MIDIOutputDevice.hpp"

#include <winrt/Windows.Devices.Midi.h>

namespace YADAW::MIDI
{
using winrt::Windows::Devices::Midi::MidiOutPort;
using winrt::Windows::Devices::Midi::IMidiOutPort;
class MIDIOutputDevice::Impl
{
public:
    static std::vector<MIDIOutputDeviceInfo> enumerateDevices();
public:
    Impl(const QString& id);
    Impl(const Impl&) = delete;
    Impl(Impl&& rhs) = delete;
    ~Impl();
private:
    IMidiOutPort midiOutPort_;
};
}

#endif //YADAW_SRC_NATIVE_MIDIOUTPUTDEVICEIMPL
