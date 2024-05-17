#if _WIN32

#include "MIDIOutputDeviceImpl.hpp"

#include "native/win/winrt/Async.hpp"
#include "native/win/winrt/QStringFromHString.hpp"
#include "util/Util.hpp"

#include <../include/winrt/Windows.Storage.Streams.h>

using winrt::Windows::Devices::Enumeration::DeviceInformation;
using winrt::Windows::Devices::Enumeration::DeviceInformationCollection;
using winrt::Windows::Devices::Enumeration::DeviceClass;
using winrt::Windows::Devices::Midi::MidiOutPort;

namespace YADAW::MIDI
{
DeviceInformationCollection midiOutputDevices_ {nullptr};
std::once_flag enumerateOutputDeviceFlag;

std::size_t MIDIOutputDevice::Impl::outputDeviceCount()
{
    std::call_once(enumerateOutputDeviceFlag,
        []()
        {
            midiOutputDevices_ = asyncResult(
                DeviceInformation::FindAllAsync(MidiOutPort::GetDeviceSelector())
            );
        }
    );
    return midiOutputDevices_.Size();
}

std::optional<DeviceInfo> MIDIOutputDevice::Impl::outputDeviceAt(std::size_t index)
{
    if(index < outputDeviceCount())
    {
        const auto& info = midiOutputDevices_.GetAt(index);
        YADAW::MIDI::DeviceInfo ret;
        ret.id = info.Id();
        ret.name = YADAW::Native::qStringFromHString(info.Name());
        return std::optional(ret);
    }
    return std::nullopt;
}
}
// TODO on Win32

#endif