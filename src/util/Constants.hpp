#ifndef YADAW_SRC_UTIL_CONSTANTS
#define YADAW_SRC_UTIL_CONSTANTS

#if __cplusplus >= 201907L
#include <numbers>
#else
#include <cmath>
#endif

namespace YADAW::Util
{
#if __cplusplus >= 201907L
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
#else

// Variable template and `constexpr` is not used for several reasons:
// - Functions in <cmath> might modify global variables like `errno`
// - Some functions in <cmath> depend on the rounding mode at runtime
template<typename T>
T pi()
{
#if(__GNUC__)
    return std::acos(-1.0l);
#else
    return std::acosl(-1.0l);
#endif
}

template<typename T>
T sqrt2()
{
#if(__GNUC__)
    return std::sqrt(2.0l);
#else
    return std::sqrtl(2.0l);
#endif
}
#endif
}

#endif // YADAW_SRC_UTIL_CONSTANTS
