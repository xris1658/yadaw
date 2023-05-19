#ifndef YADAW_AUDIO_BASE_CHRONO
#define YADAW_AUDIO_BASE_CHRONO

#include <cstddef>
#include <cstdint>

namespace YADAW::Audio::Base
{
template<std::size_t PPQ>
class Duration;

template<std::size_t PPQ>
class TimePoint
{
    static_assert(PPQ % 24 == 0);
public:
    TimePoint(std::int64_t pulse = 0U): pulse_(pulse) {}
    TimePoint(const TimePoint<PPQ>&) = default;
    TimePoint& operator=(const TimePoint<PPQ>&) = default;
    TimePoint(TimePoint<PPQ>&&) noexcept = default;
    TimePoint& operator=(TimePoint<PPQ>&&) noexcept = default;
    ~TimePoint() noexcept {}
public:
    inline std::int64_t count() const noexcept
    {
        return pulse_;
    }
    inline constexpr std::size_t ppq() const noexcept
    {
        return PPQ;
    }
public:
    TimePoint<PPQ>& operator++()
    {
        ++pulse_;
        return *this;
    }
    TimePoint<PPQ>& operator--()
    {
        --pulse_;
        return *this;
    }
    TimePoint<PPQ> operator--(int)
    {
        TimePoint<PPQ> ret = *this;
        operator--();
        return ret;
    }
    TimePoint<PPQ> operator++(int)
    {
        TimePoint<PPQ> ret = *this;
        operator++();
        return ret;
    }
    TimePoint<PPQ>& operator+=(const Duration<PPQ>& duration);
    TimePoint<PPQ>& operator-=(const Duration<PPQ>& duration);
public:
    template<std::size_t PPQ2>
    explicit operator TimePoint<PPQ2>()
    {
        return TimePoint<PPQ2>(pulse_ * PPQ2 / PPQ);
    }
private:
    std::int64_t pulse_;
};

template<std::size_t PPQ>
class Duration
{
public:
    Duration(std::int64_t duration): duration_(duration) {}
    Duration(const Duration<PPQ>&) = default;
    Duration& operator=(const Duration<PPQ>&)= default;
    Duration(Duration<PPQ>&&) noexcept = default;
    Duration& operator=(Duration<PPQ>&&) noexcept = default;
    ~Duration() noexcept {}
public:
    std::int64_t count() const
    {
        return duration_;
    }
    constexpr std::size_t ppq()
    {
        return PPQ;
    }
public:
    Duration<PPQ>& operator+=(const Duration<PPQ>& rhs)
    {
        duration_ += rhs.duration_;
        return *this;
    }
    Duration<PPQ>& operator-=(const Duration<PPQ>& rhs)
    {
        duration_ -= rhs.duration_;
        return *this;
    }
public:
    template<std::size_t PPQ2>
    explicit operator Duration<PPQ2>()
    {
        return Duration<PPQ2>(duration_ * PPQ2 / PPQ);
    }
private:
    std::int64_t duration_;
};

// TimePoint comparison
template<std::size_t PPQ>
bool operator==(const TimePoint<PPQ>& lhs, const TimePoint<PPQ>& rhs)
{
    return lhs.count() == rhs.count();
}

template<std::size_t PPQ>
bool operator!=(const TimePoint<PPQ>& lhs, const TimePoint<PPQ>& rhs)
{
    return !(lhs == rhs);
}

template<std::size_t PPQ>
bool operator<(const TimePoint<PPQ>& lhs, const TimePoint<PPQ>& rhs)
{
    return lhs.count() < rhs.count();
}

template<std::size_t PPQ>
bool operator>(const TimePoint<PPQ>& lhs, const TimePoint<PPQ>& rhs)
{
    return rhs < lhs;
}

template<std::size_t PPQ>
bool operator<=(const TimePoint<PPQ>& lhs, const TimePoint<PPQ>& rhs)
{
    return !(lhs > rhs);
}

template<std::size_t PPQ>
bool operator>=(const TimePoint<PPQ>& lhs, const TimePoint<PPQ>& rhs)
{
    return !(lhs < rhs);
}

// Duration comparison
template<std::size_t PPQ>
bool operator==(const Duration<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return lhs.count() == rhs.count();
}

template<std::size_t PPQ>
bool operator!=(const Duration<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return !(lhs == rhs);
}

template<std::size_t PPQ>
bool operator<(const Duration<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return lhs.count() < rhs.count();
}

template<std::size_t PPQ>
bool operator>(const Duration<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return rhs < lhs;
}

template<std::size_t PPQ>
bool operator<=(const Duration<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return !(lhs > rhs);
}

template<std::size_t PPQ>
bool operator>=(const Duration<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return !(lhs < rhs);
}

// TimePoint + Duration
template<std::size_t PPQ>
TimePoint<PPQ> operator+(const TimePoint<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return TimePoint<PPQ>(lhs.count() + rhs.count());
}

// TimePoint - Duration
template<std::size_t PPQ>
TimePoint<PPQ> operator-(const TimePoint<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return TimePoint<PPQ>(lhs.count() - rhs.count());
}

// TimePoint - TimePoint
template<std::size_t PPQ>
Duration<PPQ> operator-(const TimePoint<PPQ>& lhs, const TimePoint<PPQ>& rhs)
{
    return Duration<PPQ>(lhs.count() - rhs.count());
}

// Duration + Duration
template<std::size_t PPQ>
Duration<PPQ> operator+(const Duration<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return Duration<PPQ>(lhs.count() + rhs.count());
}

// Duration - Duration
template<std::size_t PPQ>
Duration<PPQ> operator-(const Duration<PPQ>& lhs, const Duration<PPQ>& rhs)
{
    return Duration<PPQ>(lhs.count() - rhs.count());
}

template<std::size_t PPQ>
TimePoint<PPQ>& TimePoint<PPQ>::operator+=(const Duration<PPQ>& duration)
{
    pulse_ += duration.count();
    return *this;
}

template<std::size_t PPQ>
TimePoint<PPQ>& TimePoint<PPQ>::operator-=(const Duration<PPQ>& duration)
{
    pulse_ -= duration.count();
    return *this;
}
}

#endif //YADAW_AUDIO_BASE_CHRONO
