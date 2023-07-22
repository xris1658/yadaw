#ifndef YADAW_SRC_UTIL_ARRAYACCESS
#define YADAW_SRC_UTIL_ARRAYACCESS

#include <cstdlib>

namespace YADAW::Util
{
template<typename T>
T* getOrNull(T* pointer, std::size_t count, std::size_t index)
{
    return index < count? pointer + index: nullptr;
}

template<typename T>
auto getOrNull(const T& container, std::size_t index)
{
    return getOrNull(container.data(), container.size(), index);
}

template<typename T>
auto getOrNull(T& container, std::size_t index)
{
    return getOrNull(container.data(), container.size(), index);
}
}

#endif // YADAW_SRC_UTIL_ARRAYACCESS
