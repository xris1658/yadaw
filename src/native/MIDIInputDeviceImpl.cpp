#include "MIDIInputDeviceImpl.hpp"

#include "native/winrt/Forward.hpp"
#include "native/winrt/QStringFromHString.hpp"
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Midi.h>
#include <winrt/Windows.Devices.Enumeration.h>

namespace YADAW::MIDI
{
using MIDIInputDeviceInfo = MIDIInputDevice::MIDIInputDeviceInfo;
using namespace winrt::Windows::Devices::Enumeration;
using winrt::Windows::Devices::Midi::MidiInPort;
using namespace YADAW::Native;

std::vector<MIDIInputDeviceInfo> MIDIInputDevice::Impl::enumerateDevices()
{
    const auto& inputDevices = DeviceInformation::FindAllAsync(MidiInPort::GetDeviceSelector()).get();
    std::vector<MIDIInputDeviceInfo> ret;
    auto size = inputDevices.Size();
    ret.reserve(size);
    for(const auto& info: inputDevices)
    {
        MIDIInputDeviceInfo emplace {qStringFromHString(info.Id()), qStringFromHString(info.Name())};
        ret.emplace_back(emplace);
    }
    return ret;
}

MIDIInputDevice::Impl::Impl(const QString& id):
    midiInPort_(nullptr)
{
    winrt::hstring idAsHString(id.toStdWString());
    midiInPort_ = std::move(MidiInPort::FromIdAsync(idAsHString).get());
}

MIDIInputDevice::Impl::~Impl()
{
    //
}
}