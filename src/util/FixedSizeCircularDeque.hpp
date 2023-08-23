#ifndef YADAW_SRC_UTIL_FIXEDSIZECIRCULARDEQUE
#define YADAW_SRC_UTIL_FIXEDSIZECIRCULARDEQUE

#include "util/IntegerRange.hpp"

#include <array>
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace YADAW::Util
{
template<typename T, std::size_t Capacity>
class FixedSizeCircularDeque
{
    using Self = FixedSizeCircularDeque<T, Capacity>;
public:
    class ConstIterator;
    class Iterator
    {
        friend class FixedSizeCircularDeque<T, Capacity>;
        friend class ConstIterator;
    public:
        Iterator() : container_(nullptr), index_(0) {}
    private:
        Iterator(Self& container, std::size_t index) : container_(&container), index_(index) {}
    public:
        Iterator(const Iterator&) = default;
        Iterator(Iterator&&) noexcept = default;
        Iterator& operator=(Iterator rhs)
        {
            std::swap(container_, rhs.container_);
            std::swap(index_, rhs.index_);
            return *this;
        }
        ~Iterator() noexcept = default;
    public:
        const T& operator*() const { assert(index_ < container_->size()); return (*container_)[index_]; };
        T& operator*() { return const_cast<T&>(static_cast<const Iterator&>(*this).operator*()); }
        const T* operator->() const { return &(operator*()); }
        T* operator->() { return const_cast<T*>(static_cast<const Self&>(*this).operator->()); }
    public:
        bool operator==(const Iterator& rhs) const { return container_ == rhs.container_ && index_ == rhs.index_; }
        bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }
        bool operator<(const Iterator& rhs) const { assert(container_ == rhs.container_); return index_ < rhs.index_; }
        bool operator>(const Iterator& rhs) const { return rhs < *this; }
        bool operator<=(const Iterator& rhs) const { return !(rhs < *this); }
        bool operator>=(const Iterator& rhs) const { return !(*this < rhs); }
    public:
        const Iterator& operator++() { ++index_; return *this; }
        const Iterator  operator++(int) { auto ret = *this; operator++(); return ret; }
        const Iterator& operator--() { --index_; return *this; }
        const Iterator  operator--(int) { auto ret = *this; operator++(); return ret; }
        const Iterator& operator+=(std::size_t offset) { index_ += offset; return *this; }
        const Iterator& operator-=(std::size_t offset) { index_ -= offset; return *this; }
        const Iterator  operator+(std::size_t offset) const { return { container_, index_ + offset }; }
        const Iterator  operator-(std::size_t offset) const { return { container_, index_ - offset }; }
        std::ptrdiff_t  operator-(const Iterator& rhs) const { return index_ - rhs.index_; } // FIXME: Deal with overflow
    private:
        Self* container_;
        std::size_t index_;
    };
    class ConstIterator
    {
        friend class FixedSizeCircularDeque<T, Capacity>;
    public:
        ConstIterator() : container_(nullptr), index_(0) {}
    private:
        ConstIterator(Self& container, std::size_t index) : container_(&container), index_(index) {}
    public:
        ConstIterator(const ConstIterator&) = default;
        ConstIterator(const Iterator& rhs): container_(rhs.container_), index_(rhs.index_) {}
        ConstIterator(ConstIterator&&) noexcept = default;
        ConstIterator& operator=(ConstIterator rhs)
        {
            std::swap(container_, rhs.container_);
            std::swap(index_, rhs.index_);
            return *this;
        }
        ConstIterator& operator=(Iterator rhs)
        {
            std::swap(container_, rhs.container_);
            std::swap(index_, rhs.index_);
            return *this;
        }
        ~ConstIterator() noexcept = default;
    public:
        const T& operator*() const { return (*container_)[index_]; };
        const T* operator->() const { return &(operator*()); }
    public:
        bool operator==(const ConstIterator& rhs) const { return container_ == rhs.container_ && index_ == rhs.index_; }
        bool operator!=(const ConstIterator& rhs) const { return !(*this == rhs); }
        bool operator<(const ConstIterator& rhs) const { assert(container_ == rhs.container_); return index_ < rhs.index_; }
        bool operator>(const ConstIterator& rhs) const { return rhs < *this; }
        bool operator<=(const ConstIterator& rhs) const { return !(rhs < *this); }
        bool operator>=(const ConstIterator& rhs) const { return !(*this < rhs); }
    public:
        const ConstIterator& operator++() { ++index_; return *this; }
        const ConstIterator  operator++(int) { auto ret = *this; operator++(); return ret; }
        const ConstIterator& operator--() { --index_; return *this; }
        const ConstIterator  operator--(int) { auto ret = *this; operator++(); return ret; }
        const ConstIterator& operator-=(std::size_t offset) { index_ -= offset; return *this; }
        const ConstIterator& operator+=(std::size_t offset) { index_ += offset; return *this; }
        const ConstIterator  operator+(std::size_t offset) const { return { container_, index_ + offset }; }
        const ConstIterator  operator-(std::size_t offset) const { return { container_, index_ - offset }; }
        std::ptrdiff_t       operator-(const Iterator& rhs) const { return index_ - rhs.index_; } // FIXME: Deal with overflow
    private:
        Self* container_;
        std::size_t index_;
    };
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
    Iterator begin() noexcept
    {
        return Iterator(*this, 0);
    }
    Iterator end() noexcept
    {
        return Iterator(*this, count_);
    }
    ConstIterator cbegin() const noexcept
    {
        return ConstIterator(*this, 0);
    }
    ConstIterator cend() const noexcept
    {
        return ConstIterator(*this, count_);
    }
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
        if constexpr(!std::is_trivially_destructible_v<T>)
        {
            FOR_RANGE0(i, count_)
            {
                (&(operator[](i)))->~T();
            }
        }
        count_ = 0;
    }
private:
    std::array<std::byte[sizeof(T)], Capacity> data_;
    std::size_t start_ = 0;
    std::size_t count_ = 0;
};
}

#endif // YADAW_SRC_UTIL_FIXEDSIZECIRCULARDEQUE
