#include "MessageImpl.hpp"

#include "native/winrt/Forward.hpp"
#include <winrt/Windows.Storage.Streams.h>

namespace YADAW::MIDI
{
using namespace winrt::Windows::Devices::Midi;
std::uint8_t Message::Impl::messageType() const
{
    return static_cast<std::uint8_t>(message_->Type());
}

std::int64_t Message::Impl::timestampInNanosecond() const
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(message_->Timestamp()).count();
}

std::uint64_t Message::Impl::dataSize() const
{
    return message_->RawData().Length();
}

std::uint8_t* Message::Impl::data() const
{
    return message_->RawData().data();
}
}