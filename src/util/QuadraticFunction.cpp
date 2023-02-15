#include "QuadraticFunction.hpp"

#include <cmath>

namespace YADAW::Util
{
QuadraticEquationRoot solveEquation(double a, double b, double c)
{
    if ((!std::isnan(a)) && (!std::isnan(b)) && (!std::isnan(c)))
    {
        double delta = b * b - 4 * a * c;
        if (delta == 0)
        {
            return {-1 * b / a * 0.5, -1 * b / a * 0.5};
        }
        if (delta > 0)
        {
            auto sqrtDelta = std::sqrt(delta);
            return {(-1 * b + sqrtDelta) / a * 0.5, (-1 * b - sqrtDelta) / a * 0.5};
        }
    }
    return {NAN, NAN};
}

double QuadraticFunction::delta() const
{
    return b * b  - 4 * a * c;
}

double QuadraticFunction::operator()(double x) const
{
    return a * x * x + b * x + c;
}
}