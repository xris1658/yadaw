#ifndef YADAW_SRC_UTIL_BATCHDISPOSER
#define YADAW_SRC_UTIL_BATCHDISPOSER

#include "PolymorphicDeleter.hpp"

#include <memory>
#include <vector>

namespace YADAW::Util
{
class BatchDisposer
{
public:
    using Object = std::unique_ptr<void, PolymorphicDeleter>;
public:
    BatchDisposer();
    ~BatchDisposer();
public:
    void addObject(Object&& obj);
private:
    std::vector<Object> disposingObjects_;
};
}

#endif // YADAW_SRC_UTIL_BATCHDISPOSER