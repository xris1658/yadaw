#ifndef YADAW_SRC_UTIL_FIBONACCIITERATOR
#define YADAW_SRC_UTIL_FIBONACCIITERATOR

#include <cstdint>
#include <iterator>
#include <utility>

namespace YADAW::Util
{
template<typename IntegerType>
class FibonacciIterator
{
public:
    static FibonacciIterator begin()
    {
        return {0, 1};
    }
public:
    FibonacciIterator& operator++()
    {
        value_.second += value_.first;
        value_.first = value_.second - value_.first;
        return *this;
    }
    FibonacciIterator& operator--()
    {
        value_.first = value_.second - value_.first;
        value_.second -= value_.first;
        return *this;
    }
    FibonacciIterator  operator++(int)
    {
        auto ret = *this;
        operator++();
        return ret;
    }
    FibonacciIterator  operator--(int)
    {
        auto ret = *this;
        operator--();
        return ret;
    }
    std::pair<IntegerType, IntegerType> operator*() const
    {
        return value_;
    }
    friend bool operator==(const FibonacciIterator<IntegerType>& lhs, const FibonacciIterator<IntegerType>& rhs)
    {
        return lhs.value_ == rhs.value_;
    }
    friend bool operator!=(const FibonacciIterator<IntegerType>& lhs, const FibonacciIterator<IntegerType>& rhs)
    {
        return !(lhs == rhs);
    }
    friend bool operator<(const FibonacciIterator& lhs, const FibonacciIterator& rhs)
    {
        return lhs.value_ < rhs.value_;
    }
    friend bool operator>(const FibonacciIterator& lhs, const FibonacciIterator& rhs)
    {
        return rhs < lhs;
    }
    friend bool operator<=(const FibonacciIterator& lhs, const FibonacciIterator& rhs)
    {
        return !(rhs < lhs);
    }
    friend bool operator>=(const FibonacciIterator& lhs, const FibonacciIterator& rhs)
    {
        return !(lhs < rhs);
    }
private:
    std::pair<IntegerType, IntegerType> value_;
};
}

namespace std
{
template<typename IntegerType>
struct iterator_traits<YADAW::Util::FibonacciIterator < IntegerType>>
{
    using difference_type = std::int64_t;
    using value_type = std::pair<IntegerType, IntegerType>;
    using reference = value_type&;
    using iterator_category = std::bidirectional_iterator_tag;
};
}

#endif //YADAW_SRC_UTIL_FIBONACCIITERATOR
