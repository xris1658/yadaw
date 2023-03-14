#ifndef YADAW_UTIL_CIRCULARDEQUE
#define YADAW_UTIL_CIRCULARDEQUE

#include <array>
#include <stdexcept>

namespace YADAW::Util
{
template<typename T, std::size_t Capacity>
class FixedSizeCircularDeque
{
    using Self = FixedSizeCircularDeque<T, Capacity>;
public:
    FixedSizeCircularDeque(): data_()
    {
        //
    }
    FixedSizeCircularDeque(const Self& rhs) = default;
    Self& operator=(const Self& rhs) = default;
    FixedSizeCircularDeque(Self&& rhs) noexcept = default;
    Self& operator=(Self&& rhs) noexcept = default;
    ~FixedSizeCircularDeque() noexcept = default;
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
            throw std::out_of_range("Error: FixedSizeCircularDeque subscript out of range");
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
    void beforePushFront()
    {
        if(start_ == 0)
        {
            start_ = Capacity - 1;
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
            if(start_ == Capacity)
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
        for(decltype(count_) i = 0; i < count_; ++i)
        {
            (&(operator[](i)))->~T();
        }
    }
private:
    std::array<std::byte[sizeof(T)], Capacity> data_;
    std::size_t start_ = 0;
    std::size_t count_ = 0;
};
}

#endif //YADAW_UTIL_CIRCULARDEQUE
