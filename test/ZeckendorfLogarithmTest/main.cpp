#include "util/IntegerRange.hpp"
#include "util/ZeckendorfLogarithm.hpp"

#include <chrono>
#include <cmath>
#include <stdexcept>

int main() try
{
    auto base0 = YADAW::Util::zeckendorfLogarithm<int>(0, 2);
    if(base0.has_value())
    {
        throw std::runtime_error("base 0 has value!");
    }
    auto base1 = YADAW::Util::zeckendorfLogarithm<int>(1, 2);
    if(base1.has_value())
    {
        throw std::runtime_error("base 1 has value!");
    }
    auto logOf0 = YADAW::Util::zeckendorfLogarithm<int>(2, 0);
    if(logOf0.has_value())
    {
        throw std::runtime_error("logOf0 has value!");
    }
    std::printf("\n");
    FOR_RANGE(i, 1, 730)
    {
        std::printf("\t%d: %d\n", i, YADAW::Util::zeckendorfLogarithm<int>(1000, i).value());
    }

}
catch(std::exception& e)
{
    std::fprintf(stderr, "Caught exception: %s\n", e.what());
}