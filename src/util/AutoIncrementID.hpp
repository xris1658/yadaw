#ifndef YADAW_SRC_UTIL_AUTOINCREMENTID
#define YADAW_SRC_UTIL_AUTOINCREMENTID

#include <cstdint>

namespace YADAW::Util
{
class AutoIncrementID
{
public:
    static constexpr std::uint64_t InvalidId = 0;
    constexpr std::uint64_t InitialId = 1;
public:
    std::uint64_t operator()();
    void reset();
private:
    std::uint64_t newId_ = InitialId;
};
}

#endif // YADAW_SRC_UTIL_AUTOINCREMENTID