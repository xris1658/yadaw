#ifndef YADAW_SRC_UTIL_POLYMORPHICDELETER
#define YADAW_SRC_UTIL_POLYMORPHICDELETER

#include <memory>
#include <utility>

namespace YADAW::Util
{
class PolymorphicDeleter
{
public:
    using DeleterFunction = void(void*);
private:
    template<typename T>
    static void deleterFunc(void* ptr)
    {
        delete static_cast<T*>(ptr);
    }
public:
    template<typename D>
    PolymorphicDeleter() noexcept: deleter_(&deleterFunc<D>) {}
    PolymorphicDeleter(const PolymorphicDeleter&) noexcept = default;
    PolymorphicDeleter& operator=(const PolymorphicDeleter&) noexcept = default;
    ~PolymorphicDeleter() noexcept = default;
    void operator()(void* pointer) const
    {
        deleter_(pointer);
    }
    DeleterFunction* getDeleter() const
    {
        return deleter_;
    }
private:
    DeleterFunction* const deleter_;
};

template<typename T>
using PMRUniquePtr = std::unique_ptr<T, PolymorphicDeleter>;

template<typename T>
PMRUniquePtr<T> createUniquePtr(T* ptr)
{
    return PMRUniquePtr<T>(ptr, PolymorphicDeleter<T>());
}


template<typename T>
PMRUniquePtr<T> createUniquePtr(std::nullptr_t)
{
    return PMRUniquePtr<T>(nullptr, PolymorphicDeleter<T>());
}
}

#endif // YADAW_SRC_UTIL_POLYMORPHICDELETER