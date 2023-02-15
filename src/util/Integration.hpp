#ifndef YADAW_MATH_INTEGRATION
#define YADAW_MATH_INTEGRATION

#include "QuadraticFunction.hpp"

namespace YADAW::Util
{
double constantIntegration(double c, double from, double to);

double linearFunctionIntegration(double k, double b, double from, double to);

double linearFunctionInvertIntegration(double k, double b, double from, double to);

double quadraticFunctionIntegration(const QuadraticFunction& function, double from, double to);

double quadraticFunctionInvertIntegration(const QuadraticFunction& function, double from, double to);
}

#endif //YADAW_MATH_INTEGRATION
