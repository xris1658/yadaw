#include "Integration.hpp"

#include <cmath>
#include <stdexcept>

namespace YADAW::Util
{
namespace Impl
{
inline double lnAbs(double x)
{
    return std::log(std::abs(x));
}
}
double constantIntegration(double c, double from, double to)
{
    return (to - from) * c;
}

double linearFunctionIntegration(double k, double b, double from, double to)
{
    return k * (to * to - from * from) * 0.5 + b * (to - from);
}

double linearFunctionInvertIntegration(double k, double b, double from, double to)
{
    if(k == 0)
    {
        return constantIntegration(1 / b, from, to);
    }
    using namespace Impl;
    return (lnAbs(k * to + b) - lnAbs(k * from + b)) / k;
}

double quadraticFunctionIntegration(const QuadraticFunction& function, double from, double to)
{
    constexpr auto oneThird = 1 / 3.0;
    const auto& [a, b, c] = function;
    return a * (to * to * to - from * from * from) * oneThird + b * (to * to - from * from) * 0.5 + c * (to - from);
}

double quadraticFunctionInvertIntegration(const QuadraticFunction& function, double from, double to)
{
    using namespace Impl;
    if(function.a == 0)
    {
        return linearFunctionInvertIntegration(function.b, function.c, from, to);
    }
    else if(function.b == 0)
    {
        const auto& [a, zero, b] = function;
        if(b == 0)
        {
            throw std::invalid_argument("");
        }
        if(b > 0)
        {
            return (
                std::atan(std::sqrt(a / b) * to)
                - std::atan(std::sqrt(a / b) * from)
            ) / std::sqrt(a * b);
        }
        else /*if(b < 0)*/
        {
            auto sqrta = std::sqrt(a);
            auto sqrtNegb = std::sqrt(b * -1);
            auto numeratorFrom   = sqrta * from - sqrtNegb;
            auto denominatorFrom = numeratorFrom + 2 * sqrtNegb;
            auto numeratorTo     = sqrta * to   - sqrtNegb;
            auto denominatorTo   = numeratorTo * 2 * sqrtNegb;
            return
            (
                lnAbs(numeratorTo / denominatorTo) - lnAbs(numeratorFrom / denominatorFrom)
            ) / std::sqrt(-1 * a * b) * 0.5;
        }
    }
    else
    {
        const auto& [a, b, c] = function;
        double delta = function.delta();
        if(delta == 0)
        {
            return (2 / (b + 2 * a * from)) - (2 / (b + 2 * a * to));
        }
        if(delta < 0)
        {
            auto sqrtNegDelta = std::sqrt(delta * -1);
            return (std::atan((2 * a * to + b) / sqrtNegDelta) - std::atan((2 * a * from + b) / sqrtNegDelta)) * 2 / sqrtNegDelta;
        }
        else
        {
            auto sqrtDelta = std::sqrt(delta);
            auto numeratorFrom   = 2 * a * from + b - sqrtDelta;
            auto denominatorFrom = numeratorFrom + 2 * sqrtDelta;
            auto numeratorTo     = 2 * a * to   + b - sqrtDelta;
            auto denominatorTo   = numeratorTo + 2 * sqrtDelta;
            return (lnAbs(numeratorTo / denominatorTo) - lnAbs(numeratorFrom / denominatorFrom)) / sqrtDelta;
        }
    }
}

}

