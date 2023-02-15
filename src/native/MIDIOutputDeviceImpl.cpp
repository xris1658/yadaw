#include "MIDIOutputDeviceImpl.hpp"

#include "native/winrt/Forward.hpp"
#include "native/winrt/QStringFromHString.hpp"
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Midi.h>
#include <winrt/Windows.Devices.Enumeration.h>

#include <utility>

namespace YADAW::MIDI
{
using MIDIOutputDeviceInfo = MIDIOutputDevice::MIDIOutputDeviceInfo;
using namespace winrt::Windows::Devices::Enumeration;
using winrt::Windows::Devices::Midi::MidiOutPort;
using namespace YADAW::Native;

std::vector<MIDIOutputDeviceInfo> MIDIOutputDevice::Impl::enumerateDevices()
{
    const auto& inputDevices = DeviceInformation::FindAllAsync(MidiOutPort::GetDeviceSelector()).get();
    std::vector<MIDIOutputDeviceInfo> ret;
    auto size = inputDevices.Size();
    ret.reserve(size);
    for(const auto& info: inputDevices)
    {
        MIDIOutputDeviceInfo emplace {qStringFromHString(info.Id()), qStringFromHString(info.Name())};
        ret.emplace_back(emplace);
    }
    return ret;
}

MIDIOutputDevice::Impl::Impl(const QString& id):
    midiOutPort_(nullptr)
{
    winrt::hstring idAsHString(id.toStdWString());
    midiOutPort_ = MidiOutPort::FromIdAsync(idAsHString).GetResults();
}

MIDIOutputDevice::Impl::~Impl()
{
    //
}
}