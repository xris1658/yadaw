#if _WIN32

#include "MIDIInputDeviceImpl.hpp"

#include "native/win/winrt/Async.hpp"
#include "native/win/winrt/QStringFromHString.hpp"
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
            midiInputDevices_ = asyncResult(
                DeviceInformation::FindAllAsync(MidiInPort::GetDeviceSelector())
            );
        }
    );
    return midiInputDevices_.Size();
}

std::optional<YADAW::MIDI::DeviceInfo>
    MIDIInputDevice::Impl::inputDeviceAt(std::size_t index)
{
    if(index < inputDeviceCount())
    {
        const auto& info = midiInputDevices_.GetAt(index);
        YADAW::MIDI::DeviceInfo ret;
        ret.id = info.Id();
        ret.name = YADAW::Native::qStringFromHString(info.Name());
        return std::optional(ret);
    }
    return std::nullopt;
}

MIDIInputDevice::Impl::Impl(const MIDIInputDevice& device, const YADAW::Native::MIDIDeviceID& id):
    midiInPort_(nullptr),
    device_(device)
{
    midiInPort_ = asyncResult(
        winrt::Windows::Devices::Midi::MidiInPort::FromIdAsync(id)
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
            auto perceivedValue = YADAW::Util::currentTimeValueInNanosecond();
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