#include "MIDIInputDeviceImpl.hpp"

#include "native/winrt/Forward.hpp"
#include "native/winrt/QStringFromHString.hpp"
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Devices.Midi.h>
#include <winrt/Windows.Devices.Enumeration.h>

namespace YADAW::MIDI
{
using MIDIInputDeviceInfo = MIDIInputDevice::MIDIInputDeviceInfo;
using namespace winrt::Windows::Devices::Enumeration;
using winrt::Windows::Devices::Midi::MidiInPort;
using winrt::Windows::Devices::Midi::MidiMessageReceivedEventArgs;
using namespace YADAW::Native;

namespace Impl
{
template<typename... Args>
void doNothing(Args&&... args) {}
}

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

void MIDIInputDevice::Impl::start(ReceiveInputFunc* const func)
{
    token_ = midiInPort_.MessageReceived([func](const MidiInPort& port, const MidiMessageReceivedEventArgs& args)
    {
        const auto& message = args.Message();
        const auto& data = message.RawData();
        func(Message{std::chrono::duration_cast<std::chrono::nanoseconds>(message.Timestamp()).count(),
            data.Length(), data.data()});
    });
}

void MIDIInputDevice::Impl::stop()
{
    midiInPort_.MessageReceived(token_);
}
}