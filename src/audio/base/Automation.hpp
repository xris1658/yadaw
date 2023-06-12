#ifndef YADAW_SRC_AUDIO_BASE_AUTOMATION
#define YADAW_SRC_AUDIO_BASE_AUTOMATION

#include "util/QuadraticFunction.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>
#include <cassert>

namespace YADAW::Audio::Base
{
class AutomationPoint
{
public:
    using TimeType = std::int64_t;
    using ValueType = double;
private:
    using Self = AutomationPoint;
public:
    AutomationPoint(std::int64_t time, double value, double curve);
    AutomationPoint(const Self&) = default;
    Self& operator=(const Self&) = default;
    ~AutomationPoint() noexcept = default;
public:
    TimeType time() const;
    void setTime(TimeType time);
    ValueType value() const;
    void setValue(ValueType value);
    double curve() const;
    void setCurve(double curve);
    bool operator==(const AutomationPoint& rhs) const;
    bool operator!=(const AutomationPoint& rhs) const;
private:
    TimeType time_;
    ValueType value_;
    double curve_;
};

bool timeFromPointIsLessThanTime(const AutomationPoint& point, const AutomationPoint::TimeType& time);

struct AutomationMembers
{
    using PointVector = std::vector<AutomationPoint>;
    PointVector points_;
    AutomationPoint::ValueType minValue_;
    AutomationPoint::ValueType maxValue_;
};

class Automation
{
public:
    using TimeType = AutomationPoint::TimeType;
    using ValueType = AutomationPoint::ValueType;
    using Self = Automation;
    using Point = AutomationPoint;
    using PointVector = AutomationMembers::PointVector;
    using PointVectorIterator = typename PointVector::iterator;
    using PointVectorConstIterator = typename PointVector::const_iterator;
public:
    Automation(double minValue = 0.0, double maxValue = 1.0);
    Automation(const Self& rhs);
    Automation(Self&& rhs) noexcept;
    Self& operator=(const Self& rhs);
    Self& operator=(Self&& rhs) noexcept;
    ~Automation() noexcept;
    PointVectorConstIterator lowerBound(const AutomationPoint::TimeType& time) const;
    PointVectorConstIterator upperBound(const AutomationPoint::TimeType& time) const;
    PointVectorIterator lowerBound(const AutomationPoint::TimeType& time);
    PointVectorIterator upperBound(const AutomationPoint::TimeType& time);
    PointVectorIterator begin() noexcept;
    PointVectorConstIterator cbegin() const noexcept;
    PointVectorIterator end() noexcept;
    PointVectorConstIterator cend() const noexcept;
    YADAW::Util::QuadraticFunction getFunction(PointVectorConstIterator left) const;
    std::size_t pointCount() const noexcept;
    bool empty() const noexcept;
    AutomationPoint::ValueType minValue() const noexcept;
    AutomationPoint::ValueType maxValue() const noexcept;
    const Point& operator[](std::size_t index) const;
    Point& operator[](std::size_t index);
    const Point& at(std::size_t index) const;
    Point& at(std::size_t index);
    std::size_t pointCountAtTime(const AutomationPoint::TimeType& time) const;
    std::size_t firstPointIndexAtTime(const AutomationPoint::TimeType& time) const;
    double operator()(const AutomationPoint::TimeType& time, std::size_t index = 0) const;
    // Inserts a point and returns the index of the point inserted.
    // If there's no point at the given time point, then the second parameter is ignored.
    // If there are points at the given time point, then the point will be inserted there according to `indexInEqualTimePoint`.
    std::size_t insertPoint(Point point, std::size_t indexInEqualTimePoint = 0);
    void deletePoint(std::size_t index);
    void setValueOfPoint(std::size_t index, double value);
    std::size_t ifSetTimeOfPoint(std::size_t index, Automation::TimeType time, std::size_t indexInEqualTimePoint = 0);
    // Move a point, then returns the new index of the point.
    // There is another parameter `indexInEqualTimePoint`,
    // meaning that this function is similar with `insertPoint`.
    std::size_t movePoint(std::size_t index, AutomationPoint::TimeType time, std::size_t indexInEqualTimePoint = 0);
    void setCurveOfPoint(std::size_t index, double curve);
    void clearPoints();
private:
    AutomationMembers members_;
    PointVector& points_ = members_.points_;
    decltype(AutomationMembers::minValue_)& minValue_ = members_.minValue_;
    decltype(AutomationMembers::maxValue_)& maxValue_ = members_.maxValue_;
};
}

#endif // YADAW_SRC_AUDIO_BASE_AUTOMATION
