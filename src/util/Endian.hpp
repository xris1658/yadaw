#ifndef YADAW_UTIL_ENDIAN
#define YADAW_UTIL_ENDIAN

#include <cstddef>

namespace YADAW::Util
{
enum class Endian
{
    UnknownEndian,
    LittleEndian,
    BigEndian
};

void reverseEndianness(void* data, std::size_t byteCount);

void reverseEndiannessCopy(void* src, std::size_t byteCount, void* dest);
}

#endif //YADAW_UTIL_ENDIAN
