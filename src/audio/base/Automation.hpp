#ifndef YADAW_SRC_AUDIO_BASE_AUTOMATION
#define YADAW_SRC_AUDIO_BASE_AUTOMATION

#include "util/QuadraticFunction.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
#include <cassert>

namespace YADAW::Audio::Base
{
class AutomationPoint
{
public:
    using Time = std::int64_t;
    using Value = double;
private:
    using Self = AutomationPoint;
public:
    AutomationPoint(std::int64_t time, double value, double curve);
    AutomationPoint(const Self&) = default;
    Self& operator=(const Self&) = default;
    ~AutomationPoint() noexcept = default;
public:
    Time time() const;
    void setTime(Time time);
    Value value() const;
    void setValue(Value value);
    double curve() const;
    void setCurve(double curve);
    bool operator==(const AutomationPoint& rhs) const;
    bool operator!=(const AutomationPoint& rhs) const;
private:
    Time time_;
    Value value_;
    double curve_;
};

class Automation
{
    using PointVector = std::vector<AutomationPoint>;
public:
    using Time = AutomationPoint::Time;
    using Value = AutomationPoint::Value;
    using Self = Automation;
    using Point = AutomationPoint;
    using PointVectorIterator = typename PointVector::iterator;
    using PointVectorConstIterator = typename PointVector::const_iterator;
public:
    Automation(Value defaultValue, Value minValue = 0.0, Value maxValue = 1.0);
    Automation(const Self& rhs);
    Automation(Self&& rhs) noexcept;
    Self& operator=(const Self& rhs);
    Self& operator=(Self&& rhs) noexcept;
    ~Automation() noexcept;
    PointVectorConstIterator lowerBound(Time time) const;
    PointVectorConstIterator upperBound(Time time) const;
    PointVectorIterator lowerBound(Time time);
    PointVectorIterator upperBound(Time time);
    PointVectorIterator begin() noexcept;
    PointVectorConstIterator begin() const noexcept;
    PointVectorConstIterator cbegin() const noexcept;
    PointVectorIterator end() noexcept;
    PointVectorConstIterator end() const noexcept;
    PointVectorConstIterator cend() const noexcept;
    YADAW::Util::QuadraticFunction getFunction(PointVectorConstIterator left) const;
    std::size_t pointCount() const noexcept;
    bool empty() const noexcept;
    Value defaultValue() const noexcept;
    bool setDefaultValue(Value defaultValue);
    Value minValue() const noexcept;
    Value maxValue() const noexcept;
    const Point& operator[](std::size_t index) const;
    Point& operator[](std::size_t index);
    const Point& at(std::size_t index) const;
    Point& at(std::size_t index);
    std::size_t pointCountAtTime(Time time) const;
    std::size_t firstPointIndexAtTime(Time time) const;
    double operator()(Time time, std::size_t index = 0) const;
    // Inserts a point and returns the index of the point inserted.
    // If there's no point at the given time point, then the second parameter is ignored.
    // If there are points at the given time point, then the point will be inserted there according to `indexInEqualTimePoint`.
    std::size_t insertPoint(Point point, std::size_t indexInEqualTimePoint = 0);
    void deletePoint(std::size_t index);
    void setValueOfPoint(std::size_t index, double value);
    std::size_t ifSetTimeOfPoint(std::size_t index, Time time, std::size_t indexInEqualTimePoint = 0);
    // Move a point, then returns the new index of the point.
    // There is another parameter `indexInEqualTimePoint`,
    // meaning that this function is similar with `insertPoint`.
    std::size_t movePoint(std::size_t index, Time time, std::size_t indexInEqualTimePoint = 0);
    void setCurveOfPoint(std::size_t index, double curve);
    void clearPoints();
private:
    PointVector points_;
    Value defaultValue_;
    Value minValue_;
    Value maxValue_;
};
}

#endif // YADAW_SRC_AUDIO_BASE_AUTOMATION
