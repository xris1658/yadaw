#ifndef YADAW_SRC_NATIVE_WIN_MIDIINPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_WIN_MIDIINPUTDEVICEIMPL

#if(WIN32)

#include "midi/MIDIInputDevice.hpp"

#include "native/win/winrt/Forward.hpp"

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Midi.h>

namespace YADAW::MIDI
{
class MIDIInputDevice::Impl
{
public:
    static std::size_t inputDeviceCount();
    static std::optional<MIDIInputDeviceInfo> inputDeviceAt(std::size_t index);
public:
    Impl(const MIDIInputDevice& device, const QString& id);
    ~Impl();
public:
    void start(ReceiveInputFunc* const func);
    void stop();
private:
    winrt::Windows::Devices::Midi::MidiInPort midiInPort_;
    std::int64_t createTime_;
    const MIDIInputDevice& device_;
    winrt::event_token eventToken_;
};
}

#endif

#endif // YADAW_SRC_NATIVE_WIN_MIDIINPUTDEVICEIMPL
