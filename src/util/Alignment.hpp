#ifndef YADAW_SRC_UTIL_ALIGNMENT
#define YADAW_SRC_UTIL_ALIGNMENT

#ifdef __cpp_lib_bitops
#include <bit>
#endif
#include <cstddef>

namespace YADAW::Util
{
namespace Impl
{
// Crappy code right here
// Use <bit> instead in C++20
template<std::size_t Size>
constexpr std::size_t align() noexcept
{
    return
        Size <= (1ULL << 0)? (1ULL << 0):
        Size <= (1ULL << 1)? (1ULL << 1):
        Size <= (1ULL << 2)? (1ULL << 2):
        Size <= (1ULL << 3)? (1ULL << 3):
        Size <= (1ULL << 4)? (1ULL << 4):
        Size <= (1ULL << 5)? (1ULL << 5):
        Size <= (1ULL << 6)? (1ULL << 6):
        Size <= (1ULL << 7)? (1ULL << 7):
        Size <= (1ULL << 8)? (1ULL << 8):
        Size <= (1ULL << 9)? (1ULL << 9):
        Size <= (1ULL << 10)? (1ULL << 10):
        Size <= (1ULL << 11)? (1ULL << 11):
        Size <= (1ULL << 12)? (1ULL << 12):
        Size <= (1ULL << 13)? (1ULL << 13):
        Size <= (1ULL << 14)? (1ULL << 14):
        Size <= (1ULL << 15)? (1ULL << 15):
        Size <= (1ULL << 16)? (1ULL << 16):
        Size <= (1ULL << 17)? (1ULL << 17):
        Size <= (1ULL << 18)? (1ULL << 18):
        Size <= (1ULL << 19)? (1ULL << 19):
        Size <= (1ULL << 20)? (1ULL << 20):
        Size <= (1ULL << 21)? (1ULL << 21):
        Size <= (1ULL << 22)? (1ULL << 22):
        Size <= (1ULL << 23)? (1ULL << 23):
        Size <= (1ULL << 24)? (1ULL << 24):
        Size <= (1ULL << 25)? (1ULL << 25):
        Size <= (1ULL << 26)? (1ULL << 26):
        Size <= (1ULL << 27)? (1ULL << 27):
        Size <= (1ULL << 28)? (1ULL << 28):
        Size <= (1ULL << 29)? (1ULL << 29):
        Size <= (1ULL << 30)? (1ULL << 30):
        Size <= (1ULL << 31)? (1ULL << 31):
        Size <= (1ULL << 32)? (1ULL << 32):
        Size <= (1ULL << 33)? (1ULL << 33):
        Size <= (1ULL << 34)? (1ULL << 34):
        Size <= (1ULL << 35)? (1ULL << 35):
        Size <= (1ULL << 36)? (1ULL << 36):
        Size <= (1ULL << 37)? (1ULL << 37):
        Size <= (1ULL << 38)? (1ULL << 38):
        Size <= (1ULL << 39)? (1ULL << 39):
        Size <= (1ULL << 40)? (1ULL << 40):
        Size <= (1ULL << 41)? (1ULL << 41):
        Size <= (1ULL << 42)? (1ULL << 42):
        Size <= (1ULL << 43)? (1ULL << 43):
        Size <= (1ULL << 44)? (1ULL << 44):
        Size <= (1ULL << 45)? (1ULL << 45):
        Size <= (1ULL << 46)? (1ULL << 46):
        Size <= (1ULL << 47)? (1ULL << 47):
        Size <= (1ULL << 48)? (1ULL << 48):
        Size <= (1ULL << 49)? (1ULL << 49):
        Size <= (1ULL << 50)? (1ULL << 50):
        Size <= (1ULL << 51)? (1ULL << 51):
        Size <= (1ULL << 52)? (1ULL << 52):
        Size <= (1ULL << 53)? (1ULL << 53):
        Size <= (1ULL << 54)? (1ULL << 54):
        Size <= (1ULL << 55)? (1ULL << 55):
        Size <= (1ULL << 56)? (1ULL << 56):
        Size <= (1ULL << 57)? (1ULL << 57):
        Size <= (1ULL << 58)? (1ULL << 58):
        Size <= (1ULL << 59)? (1ULL << 59):
        Size <= (1ULL << 60)? (1ULL << 60):
        Size <= (1ULL << 61)? (1ULL << 61):
        Size <= (1ULL << 62)? (1ULL << 62):
                              (1ULL << 63);
}
}

template<std::size_t Size>
constexpr std::size_t align = Impl::align<Size>();
}

#endif // YADAW_SRC_UTIL_ALIGNMENT
