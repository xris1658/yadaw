#if(WIN32)

#include "MIDIInputDeviceImpl.hpp"

#include "native/win/winrt/Async.hpp"
#include "util/Util.hpp"

#include <winrt/Windows.Storage.Streams.h> // IBuffer from IMidiMessage

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

MIDIInputDevice::Impl::Impl(const MIDIInputDevice& device, const QString& id):
    midiInPort_(nullptr),
    device_(device)
{
    winrt::hstring idAsHString(reinterpret_cast<const wchar_t*>(id.data()));
    midiInPort_ = asyncResult(
        winrt::Windows::Devices::Midi::MidiInPort::FromIdAsync(
            idAsHString
        )
    );
    createTime_ = YADAW::Util::currentTimeValueInNanosecond(); // Latency?
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
        [this, func](const winrt::Windows::Devices::Midi::MidiInPort& midiInPort,
            const winrt::Windows::Devices::Midi::MidiMessageReceivedEventArgs& args)
        {
            auto preceivedValue = YADAW::Util::currentTimeValueInNanosecond();
            const auto& from = args.Message();
            YADAW::MIDI::Message to {};
            to.timestampInNanoseconds = createTime_ + static_cast<std::chrono::nanoseconds>(from.Timestamp()).count(); //
            const auto& rawData = from.RawData();
            to.size = rawData.Length();
            to.data = rawData.data();
            func(device_, to);
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