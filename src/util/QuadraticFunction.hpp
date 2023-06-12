#ifndef YADAW_SRC_UTIL_QUADRATICFUNCTION
#define YADAW_SRC_UTIL_QUADRATICFUNCTION

namespace YADAW::Util
{
struct QuadraticEquationRoot
{
    double x1;
    double x2;
};

QuadraticEquationRoot solveEquation(double a, double b, double c);

struct QuadraticFunction
{
    double a;
    double b;
    double c;
    double delta() const;
    double operator()(double x) const;
};
}

#endif // YADAW_SRC_UTIL_QUADRATICFUNCTION
