#ifndef YADAW_SRC_UTIL_CIRCULARDEQUE
#define YADAW_SRC_UTIL_CIRCULARDEQUE

#include "util/IntegerRange.hpp"

#include <stdexcept>

namespace YADAW::Util
{
template<typename T>
class CircularDeque
{
    using Self = CircularDeque<T>;
public:
    CircularDeque(std::size_t capacity):
        data_(reinterpret_cast<T*>(std::malloc(capacity * sizeof(T)))),
        capacity_(capacity),
        start_(0),
        count_(0)
    {
        if(!data_)
        {
            capacity_ = 0;
        }
    }
    CircularDeque(const Self&) = delete;
    Self& operator=(const Self&) = delete;
    CircularDeque(Self&&) = delete;
    Self& operator=(Self&&) = delete;
    ~CircularDeque()
    {
        clear();
        std::free(data_);
    }
public:
    const T& front() const
    {
        return operator[](0);
    }
    T& front()
    {
        return const_cast<T&>(static_cast<const Self&>(*this).front());
    }
    const T& back() const
    {
        return operator[](count_ - 1);
    }
    T& back()
    {
        return const_cast<T&>(static_cast<const Self&>(*this).back());
    }
    const T& operator[](std::size_t index) const
    {
        return data_[(start_ + index) % capacity_];
    }
    T& operator[](std::size_t index)
    {
        return const_cast<T&>(static_cast<const Self&>(*this)[index]);
    }
    const T& at(std::size_t index) const
    {
        if(index > size())
        {
            throw std::out_of_range("Error: CircularDeque subscript out of range");
        }
        return operator[](index);
    }
    T& at(std::size_t index)
    {
        return const_cast<T&>(static_cast<const Self&>(*this).at(index));
    }
    std::size_t size() const noexcept
    {
        return count_;
    }
    std::size_t capacity() const noexcept
    {
        return capacity_;
    }
    bool empty() const noexcept
    {
        return count_ == 0;
    }
    bool full() const noexcept
    {
        return count_ == capacity_;
    }
private:
    void beforePushFront()
    {
        if(start_ == 0)
        {
            start_ = capacity_ - 1;
        }
        else
        {
            --start_;
        }
    }
public:
    bool pushFront(const T& obj)
    {
        if(!full())
        {
            beforePushFront();
            new(&data_[start_]) T(obj);
            ++count_;
            return true;
        }
        return false;
    }
    bool pushFront(T&& obj)
    {
        if(!full())
        {
            beforePushFront();
            new(&data_[start_]) T(std::move(obj));
            ++count_;
            return true;
        }
        return false;
    }
    template<typename... Args>
    bool emplaceFront(Args&&... args)
    {
        if(!full())
        {
            beforePushFront();
            new(&data_[0]) T(std::forward<Args>(args)...);
            ++count_;
            return true;
        }
        return false;
    }
    bool popFront()
    {
        if(!empty())
        {
            if(!std::is_trivially_destructible_v<T>)
            {
                (&front())->~T();
            }
            ++start_;
            if(start_ == capacity_)
            {
                start_ = 0;
            }
            --count_;
            return true;
        }
        return false;
    }
    bool pushBack(const T& obj)
    {
        if(!full())
        {
            new(&operator[](count_)) T(obj);
            ++count_;
            return true;
        }
        return false;
    }
    bool pushBack(T&& obj)
    {
        if(!full())
        {
            new(&operator[](count_)) T(std::move(obj));
            ++count_;
            return true;
        }
        return false;
    }
    template<typename... Args>
    bool emplaceBack(Args&&... args)
    {
        if(!full())
        {
            new(&operator[](count_)) T(std::forward<Args>(args)...);
            ++count_;
            return true;
        }
        return false;
    }
    bool popBack()
    {
        if(!empty())
        {
            if(!std::is_trivially_destructible_v<T>)
            {
                (&back())->~T();
            }
            --count_;
            return true;
        }
        return false;
    }
    void clear()
    {
        FOR_RANGE0(i, count_)
        {
            (&(operator[](i)))->~T();
        }
        count_ = 0;
    }
private:
    T* data_;
    std::size_t capacity_;
    std::size_t start_;
    std::size_t count_;
};
}

#endif // YADAW_SRC_UTIL_CIRCULARDEQUE
