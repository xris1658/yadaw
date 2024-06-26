#ifndef YADAW_SRC_MIDI_MESSAGE
#define YADAW_SRC_MIDI_MESSAGE

#include <cstddef>
#include <cstdint>

namespace YADAW::MIDI
{
struct Message
{
    std::int64_t timestampInNanoseconds;
    std::size_t size;
    std::uint8_t* data;
};

struct ChannelVoiceMessageHeader
{
    std::uint8_t channelAsByte;
    std::uint8_t type() const { return channelAsByte >> 4; }
    std::uint8_t channel() const { return channelAsByte & 0b00001111; }
};

inline bool isChannelVoiceMessageHeader(std::uint8_t data)
{
    return data & 0x80;
}

struct NoteOffMessage
{
    static constexpr std::uint8_t TypeId = 0x08;
    ChannelVoiceMessageHeader header;
    std::int8_t note;
    std::int8_t velocity;
};

struct NoteOnMessage
{
    static constexpr std::uint8_t TypeId = 0x09;
    ChannelVoiceMessageHeader header;
    std::uint8_t note;
    std::uint8_t velocity;
};

struct PolyKeyPressureMessage
{
    static constexpr std::uint8_t TypeId = 0x0a;
    ChannelVoiceMessageHeader header;
    std::int8_t note;
    std::int8_t pressure;
};

struct ControlChangeMessage
{
    static constexpr std::uint8_t TypeId = 0x0b;
    ChannelVoiceMessageHeader header;
    std::int8_t controlId;
    std::int8_t controlValue;
};

struct ProgramChangeMessage
{
    static constexpr std::uint8_t TypeId = 0x0c;
    ChannelVoiceMessageHeader header;
    std::int8_t programId;
};

struct ChannelPressureMessage
{
    static constexpr std::uint8_t TypeId = 0x0d;
    ChannelVoiceMessageHeader header;
    std::int8_t pressure;
};

struct PitchBendChangeMessage
{
    static constexpr std::uint8_t TypeId = 0x0e;
    ChannelVoiceMessageHeader header;
    std::int8_t pitchBendChangeLSB;
    std::int8_t pitchBendChangeMSB;
    inline std::int16_t value() const noexcept
    {
        return pitchBendChangeMSB * 128 + pitchBendChangeLSB;
    }
};
}

#endif // YADAW_SRC_MIDI_MESSAGE
