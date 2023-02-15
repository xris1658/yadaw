#ifndef YADAW_SRC_MIDI_MESSAGE
#define YADAW_SRC_MIDI_MESSAGE

#include <cstdint>

namespace YADAW::MIDI
{
class Message
{
    class Impl;
public:
    ~Message() {}
public:
    std::uint8_t messageType() const;
    std::int64_t timestampInNanosecond() const;
    std::uint64_t dataSize() const;
    std::uint8_t* data() const;
private:
    Impl* pImpl_;
};
}

#endif //YADAW_SRC_MIDI_MESSAGE
