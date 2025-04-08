#ifndef YADAW_SRC_UTIL_CDELETER
#define YADAW_SRC_UTIL_CDELETER

#include <cstdlib>
#include <memory>

namespace YADAW::Util
{
template<typename T>
class CDeleter
{
public:
    CDeleter() {}
    CDeleter(const CDeleter&) noexcept = default;
    CDeleter& operator=(const CDeleter&) noexcept = default;
    CDeleter(CDeleter&&) noexcept = default;
    CDeleter& operator=(CDeleter&&) noexcept = default;
    ~CDeleter() {}
public:
    void operator()(T* pointer) { std::free(pointer); }
};

template<typename T>
using UniquePtrWithCDeleter = std::unique_ptr<T, CDeleter<T>>;
}

#endif // YADAW_SRC_UTIL_CDELETER