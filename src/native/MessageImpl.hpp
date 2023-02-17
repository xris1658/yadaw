#ifndef YADAW_SRC_NATIVE_MESSAGEIMPL
#define YADAW_SRC_NATIVE_MESSAGEIMPL

#include "midi/Message.hpp"

#include "native/winrt/Forward.hpp"
#include <winrt/Windows.Devices.Midi.h>

namespace YADAW::MIDI
{
class Message::Impl
{
public:
    std::uint8_t messageType() const;
    std::int64_t timestampInNanosecond() const;
    std::uint64_t dataSize() const;
    std::uint8_t* data() const;
public:
    winrt::Windows::Devices::Midi::IMidiMessage* message_;
};
}

#endif //YADAW_SRC_NATIVE_MESSAGEIMPL
