#ifndef YADAW_SRC_UTIL_ALIGNHELPER
#define YADAW_SRC_UTIL_ALIGNHELPER

#include <new>
#include <type_traits>

namespace YADAW::Util
{
template<typename T>
class AlignHelper
{
protected:
    using Aligned = std::aligned_storage_t<sizeof(T), alignof(T)>;
    inline static T* fromAligned(Aligned* ptr)
    {
        return std::launder(reinterpret_cast<T*>(ptr));
    }
    inline static const T* fromAligned(const Aligned* ptr)
    {
        return std::launder(reinterpret_cast<const T*>(ptr));
    }
};
}

#endif // YADAW_SRC_UTIL_ALIGNHELPER
