#ifndef YADAW_SRC_UTIL_ENDIAN
#define YADAW_SRC_UTIL_ENDIAN

#include <algorithm>
#include <cstddef>

namespace YADAW::Util
{
enum class Endian
{
    LittleEndian,
    BigEndian
};

inline void reverseEndianness(void* data, std::size_t byteCount)
{
    std::reverse(reinterpret_cast<std::byte*>(data), reinterpret_cast<std::byte*>(data) + byteCount);
}

inline void reverseEndiannessCopy(void* src, std::size_t byteCount, void* dest)
{
    std::reverse_copy(reinterpret_cast<std::byte*>(src), reinterpret_cast<std::byte*>(src) + byteCount, reinterpret_cast<std::byte*>(dest));
}
}

#endif // YADAW_SRC_UTIL_ENDIAN
