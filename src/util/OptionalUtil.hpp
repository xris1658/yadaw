#ifndef YADAW_SRC_UTIL_OPTIONALUTIL
#define YADAW_SRC_UTIL_OPTIONALUTIL

#include <functional>
#include <optional>

template<typename T>
using OptionalRef = std::optional<std::reference_wrapper<T>>;

#endif // YADAW_SRC_UTIL_OPTIONALUTIL
