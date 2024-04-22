#include "AutoIncrementID.hpp"

namespace YADAW::Util
{
std::uint64_t AutoIncrementID::operator()()
{
    return newId_++;
}

void AutoIncrementID::reset()
{
    newId_ = InitialId;
}
}
