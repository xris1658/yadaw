#include "Util.hpp"

namespace YADAW::Util
{

std::int64_t currentTimeValueInNanosecond()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> currentTimePointInNanosecond()
{
    return std::chrono::time_point_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now()
    );
}
}