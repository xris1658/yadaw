#ifndef YADAW_SRC_UTIL_CONSTANTS
#define YADAW_SRC_UTIL_CONSTANTS

#include <numbers>

namespace YADAW::Util
{
template<typename T>
constexpr T pi()
{
    return std::numbers::pi_v<T>;
}

template<typename T>
constexpr T sqrt2()
{
    return std::numbers::sqrt2_v<T>;
}

template<typename T>
constexpr T sqrt3()
{
    return std::numbers::sqrt3_v<T>;
}
}

#endif // YADAW_SRC_UTIL_CONSTANTS
