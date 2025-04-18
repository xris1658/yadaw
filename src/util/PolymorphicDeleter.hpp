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
        delete reinterpret_cast<T*>(ptr);
    }
private:
    PolymorphicDeleter(DeleterFunction* deleter) noexcept: deleter_(deleter) {}
public:
    template<typename D>
    static PolymorphicDeleter create()
    {
        return PolymorphicDeleter(&deleterFunc<D>);
    }
    PolymorphicDeleter(const PolymorphicDeleter&) noexcept = default;
    PolymorphicDeleter& operator=(const PolymorphicDeleter&) noexcept = default;
    ~PolymorphicDeleter() noexcept = default;
    template<typename T>
    void operator()(T* pointer) const
    {
        deleter_(reinterpret_cast<void*>(pointer));
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
    return PMRUniquePtr<T>(ptr, PolymorphicDeleter::create<T>());
}

template<typename T, typename Deleter>
PMRUniquePtr<T> createPMRUniquePtr(std::unique_ptr<T, Deleter>&& ptr)
{
    return PMRUniquePtr<T>(ptr.release(), ptr.get_deleter());
}

template<typename T>
PMRUniquePtr<T> createUniquePtr(std::nullptr_t)
{
    return PMRUniquePtr<T>(nullptr, PolymorphicDeleter::create<T>());
}
}

#endif // YADAW_SRC_UTIL_POLYMORPHICDELETER