#ifndef YADAW_SRC_UTIL_AUTOINCREMENTID
#define YADAW_SRC_UTIL_AUTOINCREMENTID

#include <cstdint>

namespace YADAW::Util
{
class AutoIncrementID
{
public:
    using ID = std::uint64_t;
    static constexpr ID InvalidId = 0;
    static constexpr ID InitialId = 1;
public:
    ID operator()();
    void reset();
private:
    ID newId_ = InitialId;
};
}

#endif // YADAW_SRC_UTIL_AUTOINCREMENTID