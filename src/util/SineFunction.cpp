#include "SineFunction.hpp"

#include "util/Constants.hpp"

#include <cmath>

namespace YADAW::Util
{
double SineFunction::operator()(double x) const
{
    return amplitude * std::sin(omega * x + phi);
}

SineFunction SineFunction::derivative() const
{
    return {amplitude * phi, omega, phi - 0.5 * YADAW::Util::pi<double>()};
}
}