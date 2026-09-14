#ifndef YADAW_SRC_UTIL_INTEGERRANGE
#define YADAW_SRC_UTIL_INTEGERRANGE

#include <ranges>
#include <type_traits>

#define FOR_RANGE(var_name, first, last) \
for(auto var_name: std::ranges::iota_view< \
    std::decay_t<decltype(first)>, \
    std::decay_t<decltype(last)> \
>(first, last))
#define FOR_RANGE0(var_name, last) \
for(auto var_name: std::ranges::iota_view< \
    std::decay_t<decltype(last)>, \
    std::decay_t<decltype(last)> \
>(0, last))

#endif // YADAW_SRC_UTIL_INTEGERRANGE
