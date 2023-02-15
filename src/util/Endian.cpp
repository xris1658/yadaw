#include "Endian.hpp"

#include <algorithm>

namespace YADAW::Util
{
void reverseEndianness(void* data, std::size_t byteCount)
{
    std::reverse(reinterpret_cast<std::byte*>(data), reinterpret_cast<std::byte*>(data) + byteCount);
}

void reverseEndiannessCopy(void* src, std::size_t byteCount, void* dest)
{
    std::reverse_copy(reinterpret_cast<std::byte*>(src), reinterpret_cast<std::byte*>(src) + byteCount, reinterpret_cast<std::byte*>(dest));
}
}