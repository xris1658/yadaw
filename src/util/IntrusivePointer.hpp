#ifndef YADAW_SRC_UTIL_INTRUSIVEPOINTER
#define YADAW_SRC_UTIL_INTRUSIVEPOINTER

#include "util/IntrusiveRefCounter.hpp"

#include <type_traits>

namespace YADAW::Util
{
template<typename T>
class IntrusivePointer
{
    static_assert(std::is_base_of_v<YADAW::Util::IntrusiveRefCounter, T>,
        "Error: T is not derived from YADAW::Util::RefCounterBase");
    using Self = IntrusivePointer<T>;
public:
    IntrusivePointer(): ptr_(nullptr) {}
    IntrusivePointer(std::nullptr_t): ptr_(nullptr) {}
    explicit IntrusivePointer(T* ptr):
        ptr_(static_cast<IntrusiveRefCounter*>(ptr))
    {}
    IntrusivePointer(const Self& rhs):
        ptr_(rhs.ptr_)
    {
        ptr_->addRef();
    }
    IntrusivePointer(Self&& rhs) noexcept:
        ptr_(rhs.ptr_)
    {
        rhs.ptr_ = nullptr;
    }
    Self& operator=(const Self& rhs)
    {
        if(this != &rhs)
        {
            reset();
            ptr_ = rhs.ptr_;
            ptr_->addRef();
        }
        return *this;
    }
    Self& operator=(Self&& rhs) noexcept
    {
        if(this != &rhs)
        {
            reset();
            swap(rhs);
        }
        return *this;
    }
    ~IntrusivePointer()
    {
        reset();
    }
public:
    void reset()
    {
        if(ptr_)
        {
            ptr_->release();
            ptr_ = nullptr;
        }
    }
    void swap(Self& rhs)
    {
        std::swap(ptr_, rhs.ptr_);
    }
    T* get() const
    {
        return static_cast<T*>(ptr_);
    }
    T* operator->() const
    {
        return get();
    }
    T& operator*() const
    {
        return *get();
    }
    operator bool() const
    {
        return get() != nullptr;
    }
    friend bool operator==(const IntrusivePointer& lhs, const IntrusivePointer& rhs)
    {
        return lhs.ptr_ == rhs.ptr_;
    }
    friend bool operator!=(const IntrusivePointer& lhs, const IntrusivePointer& rhs)
    {
        return lhs.ptr_ != rhs.ptr_;
    }
    friend bool operator<(const IntrusivePointer& lhs, const IntrusivePointer& rhs)
    {
        return lhs.ptr_ < rhs.ptr_;
    }
    friend bool operator>(const IntrusivePointer& lhs, const IntrusivePointer& rhs)
    {
        return lhs.ptr_ > rhs.ptr_;
    }
    friend bool operator<=(const IntrusivePointer& lhs, const IntrusivePointer& rhs)
    {
        return lhs.ptr_ <= rhs.ptr_;
    }
    friend bool operator>=(const IntrusivePointer& lhs, const IntrusivePointer& rhs)
    {
        return lhs.ptr_ >= rhs.ptr_;
    }
private:
    IntrusiveRefCounter* ptr_;
};
}

#endif //YADAW_SRC_UTIL_INTRUSIVEPOINTER
