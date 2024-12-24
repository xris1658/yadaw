#ifndef YADAW_SRC_UTIL_LITERAL
#define YADAW_SRC_UTIL_LITERAL

#include "util/Base.hpp"

#include <array>
#include <charconv>
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
namespace Impl
{
template<typename CharType, MIDINoteNameKeyShift> inline const CharType* noteNameArray();

const char     charNoteNameWithSharp[] = "C\0\0C#\0D\0\0D#\0E\0\0F\0\0F#\0G\0\0G#\0A\0\0A#\0B\0";
const char     charNoteNameWithFlat[] = "C\0\0Db\0D\0\0Eb\0E\0\0F\0\0Gb\0G\0\0Ab\0A\0\0Bb\0B\0";
const wchar_t  wideCharNoteNameWithSharp[] = L"C\0\0C\u266f\0D\0\0D\u266f\0E\0\0F\0\0F\u266f\0G\0\0G\u266f\0A\0\0A\u266f\0B\0";
const wchar_t  wideCharNoteNameWithFlat[] = L"C\0\0D\u266d\0D\0\0E\u266d\0E\0\0F\0\0G\u266d\0G\0\0A\u266d\0A\0\0B\u266d\0B\0";
const char16_t u16CharNoteNameWithSharp[] = u"C\000\000C\u266f\000D\000\000D\u266f\000E\000\000F\000\000F\u266f\000G\000\000G\u266f\000A\000\000A\u266f\000B\000";
const char16_t u16CharNoteNameWithFlat[] = u"C\000\000D\u266d\000D\000\000E\u266d\000E\000\000F\000\000G\u266d\000G\000\000A\u266d\000A\000\000B\u266d\000B\000";
const char32_t u32CharNoteNameWithSharp[] = U"C\000\000C\u266f\000D\000\000D\u266f\000E\000\000F\000\000F\u266f\000G\000\000G\u266f\000A\000\000A\u266f\000B\000";
const char32_t u32CharNoteNameWithFlat[] = U"C\000\000D\u266d\000D\000\000E\u266d\000E\000\000F\000\000G\u266d\000G\000\000A\u266d\000A\000\000B\u266d\000B\000";

template<> inline const char*     noteNameArray<char,     MIDINoteNameKeyShift::kSharp>() { return charNoteNameWithSharp; }
template<> inline const char*     noteNameArray<char,     MIDINoteNameKeyShift::kFlat>() { return charNoteNameWithFlat; }
template<> inline const wchar_t*  noteNameArray<wchar_t,  MIDINoteNameKeyShift::kSharp>() { return wideCharNoteNameWithSharp; }
template<> inline const wchar_t*  noteNameArray<wchar_t,  MIDINoteNameKeyShift::kFlat>() { return wideCharNoteNameWithFlat; }
template<> inline const char16_t* noteNameArray<char16_t, MIDINoteNameKeyShift::kSharp>() { return u16CharNoteNameWithSharp; }
template<> inline const char16_t* noteNameArray<char16_t, MIDINoteNameKeyShift::kFlat>() { return u16CharNoteNameWithFlat; }
template<> inline const char32_t* noteNameArray<char32_t, MIDINoteNameKeyShift::kSharp>() { return u32CharNoteNameWithSharp; }
template<> inline const char32_t* noteNameArray<char32_t, MIDINoteNameKeyShift::kFlat>() { return u32CharNoteNameWithFlat; }
const char8_t  u8CharNoteNameWithSharp[] = u8"C\0\0C\u266f\0D\0\0D\u266f\0E\0\0F\0\0F\u266f\0G\0\0G\u266f\0A\0\0A\u266f\0B\0";
const char8_t  u8CharNoteNameWithFlat[] = u8"C\0\0D\u266d\0D\0\0E\u266d\0E\0\0F\0\0G\u266d\0G\0\0A\u266d\0A\0\0B\u266d\0B\0";
template<> inline const char8_t* noteNameArray<char8_t, MIDINoteNameKeyShift::kSharp>() { return u8CharNoteNameWithSharp; }
template<> inline const char8_t* noteNameArray<char8_t, MIDINoteNameKeyShift::kFlat>() { return u8CharNoteNameWithFlat; }
}
template<typename CharType>
using NoteName = std::array<CharType, 5>;

template<typename CharType>
NoteName<CharType> getNoteName(std::int8_t note, MIDINoteNameKeyShift keyShift, MIDINoteOctaveOffset offset)
{
    NoteName<CharType> ret = {0};
    const CharType* noteName1 = (keyShift == MIDINoteNameKeyShift::kSharp)?
        Impl::noteNameArray<CharType, MIDINoteNameKeyShift::kSharp>():
        Impl::noteNameArray<CharType, MIDINoteNameKeyShift::kFlat>();
    noteName1 += (note % 12) * 3;
    std::memcpy(ret.data(), noteName1, sizeof(CharType) * 3);
    auto dest = ret.data() + (ret[1] == 0? 1: 2);
    char number[2] = {0};
    std::to_chars(number, number + 2, note / 12 + YADAW::Util::underlyingValue(offset));
    dest[0] = number[0];
    dest[1] = number[1];
    return ret;
}
}

#endif // YADAW_SRC_UTIL_LITERAL
