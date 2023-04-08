#ifndef YADAW_SRC_NATIVE_MIDIINPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_MIDIINPUTDEVICEIMPL

#include "midi/Message.hpp"
#include "midi/MIDIInputDevice.hpp"
#include "native/winrt/Forward.hpp"

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
public:
    void start(ReceiveInputFunc* const func);
    void stop();
private:
    MidiInPort midiInPort_;
    winrt::event_token token_;
};
}

#endif //YADAW_SRC_NATIVE_MIDIINPUTDEVICEIMPL
