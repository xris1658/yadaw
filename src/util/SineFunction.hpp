#ifndef YADAW_SRC_UTIL_SINEFUNCTION
#define YADAW_SRC_UTIL_SINEFUNCTION

namespace YADAW::Util
{
struct SineFunction
{
    double amplitude;
    double omega;
    double phi;
    SineFunction(): amplitude(0), omega(0), phi(0) {}
    SineFunction(double amplitude, double omega, double phi):
        amplitude(amplitude), omega(omega), phi(phi) {}
    double operator()(double x) const;
    SineFunction derivative() const;
};
}

#endif // YADAW_SRC_UTIL_SINEFUNCTION
