#ifndef YADAW_SRC_UTIL_INTEGERRANGE
#define YADAW_SRC_UTIL_INTEGERRANGE

#include <iterator>

namespace YADAW::Util
{
// Convenience class for replacing manual `for` loop.
// Generates similar (or even the same) code as the equivalent `for` loop when
// optimization is enabled (tested with O1, O2 and O3).
template<typename T>
class IntegerRange
{
    using Self = IntegerRange<T>;
public:
    class Iterator
    {
        using Self = Iterator;
    public: // For generating std::iterator_traits
        using difference_type = T;
        using value_type = T;
        using pointer = void;
        using reference = const T&;
        using iterator_category = std::random_access_iterator_tag;
    public:
        Iterator(T value): value_(value) {}
        Iterator(const Self&) = default;
        Iterator(Self&&) noexcept = default;
        Self& operator=(const Self&) = default;
        Self& operator=(Self&&) noexcept = default;
        ~Iterator() noexcept = default;
    public:
        T operator*() const
        {
            return value_;
        }
        Self& operator++()
        {
            ++value_;
            return *this;
        }
        Self& operator--()
        {
            --value_;
            return *this;
        }
        Self operator++(int)
        {
            Self ret = *this;
            operator++();
            return ret;
        }
        Self operator--(int)
        {
            Self ret = *this;
            operator--();
            return ret;
        }
        Self& operator+=(T value)
        {
            value_ += value;
            return *this;
        }
        Self& operator-=(T value)
        {
            value_ -= value;
            return *this;
        }
        friend Self operator+(const Self self, T value)
        {
            return {self.value_ + value};
        }
        friend Self operator+(T value, const Self self)
        {
            return {value + self.value_};
        }
        friend Self operator-(const Self self, T value)
        {
            return {self.value_ - value};
        }
        friend T operator-(const Self lhs, const Self rhs)
        {
            return *lhs - *rhs;
        }
        friend bool operator==(const Iterator lhs, const Iterator rhs)
        {
            return lhs.value_ == rhs.value_;
        }
        friend bool operator!=(const Iterator lhs, const Iterator rhs)
        {
            return lhs.value_ != rhs.value_;
        }
        friend bool operator<(const Iterator lhs, const Iterator rhs)
        {
            return lhs.value_ < rhs.value_;
        }
        friend bool operator>(const Iterator lhs, const Iterator rhs)
        {
            return lhs.value_ > rhs.value_;
        }
        friend bool operator<=(const Iterator lhs, const Iterator rhs)
        {
            return lhs.value_ <= rhs.value_;
        }
        friend bool operator>=(const Iterator lhs, const Iterator rhs)
        {
            return lhs.value_ >= rhs.value_;
        }
    private:
        T value_;
    };
public:
    IntegerRange(T first, T last): first_(first), last_(last) {}
    IntegerRange(T last): first_(T(0)), last_(last) {}
    IntegerRange(const Self&) = default;
    IntegerRange(Self&&) noexcept = default;
    Self& operator=(const Self&) = default;
    Self& operator=(Self&&) noexcept = default;
    ~IntegerRange() noexcept = default;
public:
    Iterator begin() const noexcept
    {
        return {first_};
    }
    Iterator end() const noexcept
    {
        return {last_};
    }
private:
    T first_;
    T last_;
};
}

#define FOR_RANGE(var_name, first, last) for(auto var_name: YADAW::Util::IntegerRange<std::common_type_t<decltype(first), decltype(last)>>(first, last))
#define FOR_RANGE0(var_name, last) for(auto var_name: YADAW::Util::IntegerRange(last))

#endif // YADAW_SRC_UTIL_INTEGERRANGE
