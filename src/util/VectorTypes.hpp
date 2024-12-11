#ifndef YADAW_SRC_UTIL_VECTORTYPES
#define YADAW_SRC_UTIL_VECTORTYPES

#include <type_traits>
#include <vector>

template<typename T>
using Vector2D = std::vector<std::vector<T>>;

template<typename T>
using Vector3D = std::vector<std::vector<std::vector<T>>>;

template<typename... Args>
using Vec = std::conditional_t<
    sizeof...(Args) == 1,
    std::vector<Args...>,
    std::vector<
        std::tuple<Args...>
    >
>;

#endif // YADAW_SRC_UTIL_VECTORTYPES
