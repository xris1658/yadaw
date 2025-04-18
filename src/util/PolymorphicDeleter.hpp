#ifndef YADAW_SRC_UTIL_POLYMORPHICDELETER
#define YADAW_SRC_UTIL_POLYMORPHICDELETER

#include <functional>
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
    PolymorphicDeleter(std::function<DeleterFunction>&& deleter): deleter_(std::move(deleter)) {}
public:
    template<typename T, typename Deleter>
    static PolymorphicDeleter create(Deleter&& deleter = std::default_delete<T>())
    {
        return PolymorphicDeleter(
            [d = std::forward<Deleter>(deleter)](void* ptr)
            {
                d(static_cast<T*>(ptr));
            }
        );
    }
    PolymorphicDeleter(const PolymorphicDeleter&) noexcept = default;
    PolymorphicDeleter& operator=(const PolymorphicDeleter&) noexcept = default;
    ~PolymorphicDeleter() noexcept = default;
    template<typename T>
    void operator()(T* pointer) const
    {
        deleter_(reinterpret_cast<void*>(pointer));
    }
private:
    std::function<DeleterFunction> deleter_;
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
    return PMRUniquePtr<T>(
        ptr.release(),
        PolymorphicDeleter::create<T>(ptr.get_deleter())
    );
}

template<typename T>
PMRUniquePtr<T> createUniquePtr(std::nullptr_t)
{
    return PMRUniquePtr<T>(nullptr, PolymorphicDeleter::create<T>());
}
}

#endif // YADAW_SRC_UTIL_POLYMORPHICDELETER