#ifndef YADAW_UTIL_CIRCULARDEQUE
#define YADAW_UTIL_CIRCULARDEQUE

#include <array>
#include <stdexcept>

namespace YADAW::Util
{
template<typename T, std::size_t Capacity>
class CircularDeque
{
    using Self = CircularDeque<T, Capacity>;
public:
    CircularDeque(): data_()
    {
        //
    }
    CircularDeque(const Self& rhs) = default;
    Self& operator=(const Self& rhs) = default;
    CircularDeque(Self&& rhs) noexcept = default;
    Self& operator=(Self&& rhs) noexcept = default;
    ~CircularDeque() noexcept = default;
public:
    const T& front() const
    {
        return reinterpret_cast<const T&>(data_[start_]);
    }
    T& front()
    {
        return const_cast<T&>(static_cast<const Self&>(*this).front());
    }
    const T& back() const
    {
        return reinterpret_cast<const T&>(data_[(start_ + count_ - 1) % Capacity]);
    }
    T& back()
    {
        return const_cast<T&>(static_cast<const Self&>(*this).back());
    }
    const T& operator[](std::size_t index) const
    {
        return reinterpret_cast<const T&>(data_[(start_ + index) % Capacity]);
    }
    T& operator[](std::size_t index)
    {
        return const_cast<T&>(static_cast<const Self&>(*this)[index]);
    }
    const T& at(std::size_t index) const
    {
        if(index >= count_)
        {
            throw std::out_of_range("");
        }
        return operator[](index);
    }
    T& at(std::size_t index)
    {
        return const_cast<T&>(static_cast<const Self&>(*this).at(index));
    }
    std::size_t size() const
    {
        return count_;
    }
    std::size_t capacity() const
    {
        return Capacity;
    }
    bool empty() const
    {
        return count_ == 0;
    }
    bool full() const
    {
        return count_ == Capacity;
    }
private:
    std::size_t decreaseStart()
    {
        if(full())
        {
            throw std::overflow_error("deque is full");
        }
        if(start_ == 0)
        {
            return Capacity - 1;
        }
        else
        {
            return start_ - 1;
        }
    }
public:
    void pushFront(const T& data)
    {
        auto newStart = decreaseStart();
        new(&(data_[newStart])) T(data);
        start_ = newStart;
        ++count_;
    }
    void pushFront(T&& data)
    {
        auto newStart = decreaseStart();
        new(&(data_[newStart])) T(std::move(data));
        start_ = newStart;
        ++count_;
    }
    template<typename... Args>
    void emplaceFront(Args&&... args)
    {
        auto newStart = decreaseStart();
        new(&(data_[start_])) T(std::forward<Args>(args)...);
        start_ = newStart;
        ++count_;
    }
    void popFront()
    {
        if(empty())
        {
            throw std::underflow_error("deque is empty");
        }
        (&(front()))->~T();
        --count_;
        if(count_ == 0)
        {
            start_ = 0;
        }
        else
        {
            start_ = (start_ + 1) % Capacity;
        }
    }
    void pushBack(const T& data)
    {
        if(full())
        {
            throw std::overflow_error("deque is full");
        }
        new(&operator[]((start_ + count_) % Capacity)) T(data);
        ++count_;
    }
    void pushBack(T&& data)
    {
        if(full())
        {
            throw std::overflow_error("deque is full");
        }
        new(&operator[]((start_ + count_) % Capacity)) T(std::move(data));
        ++count_;
    }
    template<typename... Args>
    void emplaceBack(Args&&... args)
    {
        if(full())
        {
            throw std::overflow_error("deque is full");
        }
        new(&operator[]((start_ + count_) % Capacity)) T(std::forward<Args>(args)...);
        ++count_;
    }
    void popBack()
    {
        if(empty())
        {
            throw std::underflow_error("deque is empty");
        }
        (&(back()))->~T();
        --count_;
        if(count_ == 0)
        {
            start_ = 0;
        }
    }
    void clear()
    {
        for(auto i = 0; i < count_; ++i)
        {
            (&(operator[](i)))->~T();
        }
        start_ = 0;
        count_ = 0;
    }
private:
    std::array<std::byte[sizeof(T)], Capacity> data_;
    std::size_t start_ = 0;
    std::size_t count_ = 0;
};
}

#endif //YADAW_UTIL_CIRCULARDEQUE
