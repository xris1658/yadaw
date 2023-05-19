#ifndef YADAW_AUDIO_BASE_TEMPOAUTOMATION
#define YADAW_AUDIO_BASE_TEMPOAUTOMATION

#include "audio/base/Automation.hpp"
#include "audio/base/Chrono.hpp"
#include "util/Integration.hpp"
#include "util/QuadraticFunction.hpp"

#include <cmath>
#include <stdexcept>

namespace YADAW::Audio::Base
{
constexpr auto minimumTempo = 30.0;
constexpr auto maximumTempo = 300.0;

template<std::size_t PPQ>
using TempoAutomationPoint = AutomationPoint;

template<std::size_t PPQ>
class TempoAutomation: public Automation
{
    using Base = Automation;
public:
    TempoAutomation(): Base(minimumTempo, maximumTempo) {}
private:
    // Returns the elapsed time in second, given the tempo and the elapsed time in pulse
    double secondElapsed(double bpm, const Duration<PPQ>& duration) const
    {
        return duration.count() / (bpm * PPQ) * 60.0;
    }
private:
    // Returns the elpased time in second, given a segment of automation curve and the start / stop time point in pulse.
    // This function assumes that the start / stop point is in the given segment.
    double secondElapsed(typename Base::PointVectorConstIterator left, const TimePoint<PPQ>& from, const TimePoint<PPQ>& to) const
    {
        auto function = Base::getFunction(left);
        function.a *= PPQ;
        function.b *= PPQ;
        function.c *= PPQ;
        double ret = YADAW::Util::quadraticFunctionInvertIntegration(
            function, from.count(), to.count()
        ) * 60.0;
        return ret;
    }
public:
    // Returns the PPQ of the point.
    constexpr std::size_t ppq() const
    {
        return PPQ;
    }
    // Returns the time elapsed in second, given the start and stop time in pulse.
    double secondElapsed(const TimePoint<PPQ>& from, const TimePoint<PPQ>& to) const
    {
        // `to` is ahead of `from`
        if(from > to)
        {
            return secondElapsed(to, from) * -1.0;
        }
        if(from == to)
        {
            return 0;
        }
        double ret = 0;
        auto notBeforeStart = Base::lowerBound(from.count());
        auto notBeforeEnd = Base::lowerBound(to.count());
        // --from--to--point--
        if (notBeforeStart == notBeforeEnd)
        {
            if (notBeforeStart == Base::cbegin())
            {
                ret = secondElapsed(notBeforeStart->value(), to - from);
                return ret;
            }
            else
            {
                auto beforeStart = notBeforeStart - 1;
                ret = secondElapsed(beforeStart, from, to);
                return ret;
            }
        }
        auto afterStart = Base::upperBound(from.count());
        // auto afterEnd = Base::upperBound(to.count());
        if (afterStart == Base::cend() /*&& afterEnd == Base::cend()*/)
        {
            ret = secondElapsed((Base::cend() - 1)->value(), to - from);
            return ret;
        }
        // --from--point--point--to--
        // Step 1 of 3: from--
        if(notBeforeStart == Base::cbegin())
        {
            ret += secondElapsed(notBeforeStart->value(), notBeforeStart->time() - from.count());
        }
        else
        {
            ret += secondElapsed(notBeforeStart - 1, from, notBeforeStart->time());
        }
        auto beforeEnd = notBeforeEnd - 1;
        // Step 2 of 3: --point--point--
        for(auto it = notBeforeStart; it < beforeEnd; ++it)
        {
            ret += secondElapsed(it, it->time(), (it + 1)->time());
        }
        // Step 3 of 3: --to
        if(notBeforeEnd == Base::cend())
        {
            ret += secondElapsed(beforeEnd->value(), to.count() - beforeEnd->time());
        }
        else
        {
            ret += secondElapsed(beforeEnd, beforeEnd->time(), to);
        }
        return ret;
    }
    // Returns the time elapsed from 0 in second, given the stop time in pulse.
    double secondElapsedFromZero(const TimePoint<PPQ>& to) const
    {
        return secondElapsed(TimePoint<PPQ>(0), to);
    }
    // Returns the time elapsed in the given pulse in second.
    double secondElapsedInPulse(const TimePoint<PPQ>& pulse) const
    {
        return secondElapsed(pulse, TimePoint<PPQ>(pulse.count() + 1));
    }
    // Returns the time point in pulse, given the start time point in pulse and the time elapsed in second.
    Duration<PPQ> pulseElapsedFrom(const TimePoint<PPQ>& from, double second, double precision = 60.0 / YADAW::Audio::Base::maximumTempo / static_cast<double>(PPQ)) const
    {
        if(second == 0)
        {
            return Duration<PPQ>(from.count());
        }
        // Behind `from`, or matches `from`
        auto lowerBound = Base::lowerBound(from.count);
        double sec = from == lowerBound->time()? 0: secondElapsed(from, lowerBound->time());
        for(auto i = lowerBound; i != Base::cend() - 1; ++i)
        {
            auto leftTime = i->time();
            auto durationFromThisToNext = secondElapsed(i, leftTime, (i + 1)->time());
            // --o--o--x--
            if(sec + durationFromThisToNext < second)
            {
                sec += durationFromThisToNext;
            }
            // --o--o(x)--
            else if(sec + durationFromThisToNext == second)
            {
                return Duration<PPQ>((i + 1)->time());
            }
            // --o--x--o--
            else
            {
                // If the tempo of two points are equal, then no iteration is needed.
                if(i->value() == (i + 1)->value())
                {
                    auto pulsePerSecond = i->value() * PPQ / 60.0;
                    return Duration<PPQ>(leftTime.count() + pulsePerSecond * (second - sec));
                }
                else
                {
                    auto pointIteratorAtRight = i + 1;
                    auto remain = second - sec;
                    double leftRatio = 0.0;
                    double rightRatio = 1.0;
                    constexpr double half = 0.5;
                    // Dividing by 2.0 is slower than multiplying 0.5
                    double delta = half;
                    // (leftRatio + rightRatio) / 2
                    double ratio = (leftRatio + rightRatio) * half;
                    auto rightTime = pointIteratorAtRight->time() - leftTime;
                    auto timePoint = leftTime + rightTime * ratio;
                    auto leftRange = leftTime + rightTime * leftRatio;
                    auto rightRange = leftTime + rightTime * rightRatio;
                    while(rightRange - leftRange >= 1)
                    {
                        // Behind the margin of error
                        if(remain - secondElapsed(i, static_cast<double>(leftTime), timePoint) > precision * half)
                        {
                            leftRatio += delta;
                        }
                        // Ahead of
                        else if(secondElapsed(i, static_cast<double>(leftTime), timePoint) - remain > precision * half)
                        {
                            rightRatio -= delta;
                        }
                        // Within
                        else
                        {
                            break;
                        }
                        delta *= half;
                        ratio = (leftRatio + rightRatio) * half;
                        timePoint = leftTime + rightTime * ratio;
                        leftRange = leftTime + rightTime * leftRatio;
                        rightRange = leftTime + rightTime * rightRatio;
                    }
                    auto floor = Duration<PPQ>(std::floor(timePoint));
                    auto ceiling = Duration<PPQ>(std::ceil(timePoint));
                    auto ret = floor;
                    auto secondFloor = secondElapsed(from, floor);
                    auto secondCeiling = secondElapsed(from, ceiling);
                    if (std::floor(timePoint) < leftRange || second - secondFloor >= secondCeiling - second)
                    {
                        ret = ceiling;
                    }
                    auto secondElapsedToRet = secondElapsed(from, TimePoint<PPQ>(ret.count()));
                    auto retUp = Duration<PPQ>(ret.count() + 1);
                    auto secondElapsedToRetUp = secondElapsed(from, TimePoint<PPQ>(retUp.count()));
                    if(std::abs(secondElapsedToRetUp - second) < std::abs(secondElapsedToRet - second))
                    {
                        ret = retUp;
                    }
                    if(ret.count() != 0)
                    {
                        auto retDown = Duration<PPQ>(ret.count() - 1);
                        auto secondElapsedToRetDown = secondElapsed(from, TimePoint<PPQ>(retDown.count()));
                        if(std::abs(secondElapsedToRetDown - second) < std::abs(secondElapsedToRet - second))
                        {
                            ret = retDown;
                        }
                    }
                    return ret;
                }
            }
        }
        // The pulse is at back of all points
        auto remain = second - sec;
        auto pulsePerSecond = (Base::cend() - 1)->value() * PPQ / 60.0;
        auto ret = Duration<PPQ>(((Base::cend() - 1)->time() - from).count() + pulsePerSecond * remain);
        return ret;
    }
    // Returns the time elapsed from 0 in pulse, given the stop time in second.
    Duration<PPQ> pulseAtTimePoint(double second, double precision = 60.0 / YADAW::Audio::Base::maximumTempo / static_cast<double>(PPQ)) const
    {
        return pulseElapsedFrom(TimePoint<PPQ>(0U), second, precision);
    }
};

template<std::size_t PPQ>
double secondElapsed(double tempo, const Duration<PPQ>& duration)
{
    return 60.0 * duration.count() / (tempo * PPQ);
}

template<std::size_t PPQ>
Duration<PPQ> pulseElapsed(double tempo, double second)
{
    return second * tempo * PPQ / 60.0;
}
}

#endif //YADAW_AUDIO_BASE_TEMPOAUTOMATION
