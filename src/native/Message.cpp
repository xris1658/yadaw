#include "midi/Message.hpp"

#include "native/MessageImpl.hpp"

namespace YADAW::MIDI
{
std::uint8_t Message::messageType() const
{
    return pImpl_->messageType();
}

std::int64_t Message::timestampInNanosecond() const
{
    return pImpl_->timestampInNanosecond();
}

std::uint64_t Message::dataSize() const
{
    return pImpl_->dataSize();
}

std::uint8_t* Message::data() const
{
    return pImpl_->data();
}
}