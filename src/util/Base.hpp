#ifndef YADAW_SRC_UTIL_BASE
#define YADAW_SRC_UTIL_BASE

#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <tuple>

namespace YADAW::Util
{
// null-terminator is not included
template<typename T, std::size_t N>
constexpr std::size_t stringLength(const T(&)[N]) noexcept
{
    return N - 1;
}

// std::memset
template<typename T>
void memoryReset(T* const address)
{
    std::memset(address, 0, sizeof(T));
}

template<typename T>
auto underlyingValue(T enumerate)
{
    static_assert(std::is_enum_v<T>);
    return static_cast<std::underlying_type_t<T>>(enumerate);
}
}

#endif //  YADAW_SRC_UTIL_BASE
