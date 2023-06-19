#ifndef YADAW_SRC_UTIL_UTIL
#define YADAW_SRC_UTIL_UTIL

#include <chrono>

namespace YADAW::Util
{
std::int64_t currentTimeValueInNanosecond();

std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> currentTimePointInNanosecond();
}
#endif // YADAW_SRC_UTIL_UTIL
