#if(WIN32)

#include "MIDIInputDeviceImpl.hpp"

#include "native/win/winrt/Async.hpp"

using winrt::Windows::Devices::Enumeration::DeviceInformation;
using winrt::Windows::Devices::Enumeration::DeviceInformationCollection;
using winrt::Windows::Devices::Enumeration::DeviceClass;
using winrt::Windows::Devices::Midi::MidiInPort;

namespace YADAW::MIDI
{
DeviceInformationCollection midiInputDevices_ {nullptr};
std::once_flag enumerateInputDeviceFlag;

std::size_t MIDIInputDevice::Impl::inputDeviceCount()
{
    std::call_once(enumerateInputDeviceFlag,
        []()
        {
            midiInputDevices_ = DeviceInformation::FindAllAsync(MidiInPort::GetDeviceSelector()).get();
        }
    );
    return midiInputDevices_.Size();
}

std::optional<YADAW::MIDI::MIDIInputDevice::MIDIInputDeviceInfo>
    MIDIInputDevice::Impl::inputDeviceAt(std::size_t index)
{
    if(index < inputDeviceCount())
    {
        const auto& info = midiInputDevices_.GetAt(index);
        YADAW::MIDI::MIDIInputDevice::MIDIInputDeviceInfo ret;
        ret.id = QString::fromWCharArray(info.Id().data());
        ret.name = QString::fromWCharArray(info.Name().data());
        return std::optional<MIDIInputDeviceInfo>(ret);
    }
    return std::nullopt;
}

MIDIInputDevice::Impl::Impl(const QString& id): midiInPort_(nullptr)
{
    winrt::hstring idAsHString(reinterpret_cast<const wchar_t*>(id.data()));
    midiInPort_ = asyncResult(
        winrt::Windows::Devices::Midi::MidiInPort::FromIdAsync(
            idAsHString
        )
    );
}

MIDIInputDevice::Impl::~Impl()
{
    if(eventToken_)
    {
        stop();
    }
}

void MIDIInputDevice::Impl::start(MIDIInputDevice::ReceiveInputFunc* const func)
{
    eventToken_ = midiInPort_.MessageReceived(
        [func](const winrt::Windows::Devices::Midi::MidiInPort& midiInPort,
            const winrt::Windows::Devices::Midi::MidiMessageReceivedEventArgs& args)
        {
            YADAW::MIDI::Message message {};
            // TODO: convert args.Message() to YADAW::MIDI::Message
            func(message);
        }
    );
}

void MIDIInputDevice::Impl::stop()
{
    midiInPort_.MessageReceived(eventToken_);
    eventToken_ = {};
}
}

#endif