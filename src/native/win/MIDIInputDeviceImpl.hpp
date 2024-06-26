#ifndef YADAW_SRC_NATIVE_WIN_MIDIINPUTDEVICEIMPL
#define YADAW_SRC_NATIVE_WIN_MIDIINPUTDEVICEIMPL

#if _WIN32

#include "midi/DeviceInfo.hpp"
#include "midi/MIDIInputDevice.hpp"

#include "native/win/winrt/Forward.hpp"

#include <../include/winrt/base.h>
#include <../include/winrt/Windows.Foundation.h>
#include <../include/winrt/Windows.Foundation.Collections.h>
#include <../include/winrt/Windows.Devices.Enumeration.h>
#include <../include/winrt/Windows.Devices.Midi.h>

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
    winrt::Windows::Devices::Midi::MidiInPort midiInPort_;
    std::int64_t createTime_;
    const MIDIInputDevice& device_;
    winrt::event_token eventToken_;
};
}

#endif

#endif // YADAW_SRC_NATIVE_WIN_MIDIINPUTDEVICEIMPL
