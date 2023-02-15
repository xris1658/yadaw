#ifndef YADAW_UTIL_STOPWATCH
#define YADAW_UTIL_STOPWATCH

#include "native/Native.hpp"

#include <tuple>

namespace YADAW::Util
{
template<typename ReturnType, typename... Args>
using Function = ReturnType(Args...);

template<typename Function, typename... Args, typename ReturnType = std::result_of_t<Function(Args...)>>
std::tuple<ReturnType, std::int64_t> stopwatch(Function&& function, Args&&... args)
{
    auto start = YADAW::Native::currentTimeValueInNanosecond();
    auto ret = std::make_tuple<ReturnType, std::int64_t>(
        std::forward<Function>(function)(std::forward<Args>(args)...), 0);
    // Make sure the time is retrieved AFTER the result is valid
    // In MSVC, the final object in the tuple will be initialized first, since std::tuple<A, B...>
    // derives from std::tuple<B...> in MSVC.
    // There's a better solution in terms of initializing multiple objects in the tuple in a fixed
    // order: https://stackoverflow.com/questions/49414550
    auto& [result, time] = ret;
    time = YADAW::Native::currentTimeValueInNanosecond() - start;
    return ret;
}

template<typename... Args>
std::int64_t stopwatchVoid(Function<void, Args...>&& function, Args&&... args)
{
    auto start = YADAW::Native::currentTimeValueInNanosecond();
    std::forward<Function<void, Args...>>(function)(std::forward<Args>(args)...);
    return YADAW::Native::currentTimeValueInNanosecond() - start;
}
}

#endif //YADAW_UTIL_STOPWATCH
