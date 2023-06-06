#ifndef YADAW_AUDIO_UTIL_LITERAL
#define YADAW_AUDIO_UTIL_LITERAL

#include <array>
#include <cstdint>

namespace YADAW::Util
{
enum class MIDINoteOctaveOffset
{
    kYamaha = -2,
    kRoland = -1,
    kNoOffset = 0
};

enum class MIDINoteNameKeyShift
{
    kSharp,
    kFlat
};

template<typename CharType>
using NoteName = std::array<CharType, 5>;

template<typename CharType>
NoteName<CharType> getNoteName(std::int8_t note, MIDINoteNameKeyShift keyShift, MIDINoteOctaveOffset offset = MIDINoteOctaveOffset::kYamaha);

extern template NoteName<char>     getNoteName(std::int8_t, MIDINoteNameKeyShift, MIDINoteOctaveOffset);
extern template NoteName<wchar_t>  getNoteName(std::int8_t, MIDINoteNameKeyShift, MIDINoteOctaveOffset);
extern template NoteName<char16_t> getNoteName(std::int8_t, MIDINoteNameKeyShift, MIDINoteOctaveOffset);
extern template NoteName<char32_t> getNoteName(std::int8_t, MIDINoteNameKeyShift, MIDINoteOctaveOffset);
#if __cplusplus >= 202002L
extern template NoteName<char8_t> getNoteName(std::int8_t, MIDINoteNameKeyShift, MIDINoteOctaveOffset);
#endif
}

#endif //YADAW_AUDIO_UTIL_LITERAL
