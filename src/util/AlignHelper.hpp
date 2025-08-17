#ifndef YADAW_SRC_UTIL_ALIGNHELPER
#define YADAW_SRC_UTIL_ALIGNHELPER

#include <new>
#include <type_traits>

namespace YADAW::Util
{
#ifdef __cpp_lib_hardware_interference_size
inline constexpr auto HWTrueSharingMaxSize   = std::hardware_constructive_interference_size;
inline constexpr auto HWFalseSharingMinSize  = std::hardware_destructive_interference_size;
#else
inline constexpr std::size_t HWTrueSharingMaxSize  = 64;
inline constexpr std::size_t HWFalseSharingMinSize = 64;
#endif
template<typename T>
using AlignedStorage = std::aligned_storage_t<sizeof(T), alignof(T)>;

template<typename T>
class AlignHelper
{
protected:
    using Aligned = AlignedStorage<T>;
public:
    inline static T* fromAligned(AlignedStorage<T>* ptr)
    {
        return std::launder(reinterpret_cast<T*>(ptr));
    }
    inline static const T* fromAligned(const AlignedStorage<T>* ptr)
    {
        return std::launder(reinterpret_cast<const T*>(ptr));
    }
};
}

#endif // YADAW_SRC_UTIL_ALIGNHELPER
