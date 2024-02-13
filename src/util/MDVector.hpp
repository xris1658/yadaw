#ifndef YADAW_SRC_UTIL_MDVECTOR
#define YADAW_SRC_UTIL_MDVECTOR

#include <vector>

template<typename T>
using Vector2D = std::vector<std::vector<T>>;

template<typename T>
using Vector3D = std::vector<std::vector<std::vector<T>>>;

#endif // YADAW_SRC_UTIL_MDVECTOR
