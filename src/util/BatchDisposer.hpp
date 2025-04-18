#ifndef YADAW_SRC_UTIL_BATCHDISPOSER
#define YADAW_SRC_UTIL_BATCHDISPOSER

#include "PolymorphicDeleter.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace YADAW::Util
{
class BatchDisposer
{
public:
    using Object = YADAW::Util::PMRUniquePtr<void>;
    using PreDisposeCallback = void();
public:
    BatchDisposer(std::function<PreDisposeCallback>&& callback);
    ~BatchDisposer();
public:
    template<typename T, typename Deleter>
    void addObject(std::unique_ptr<T, Deleter>&& obj)
    {
        doAddObject(YADAW::Util::createPMRUniquePtr(std::move(obj)));
    }
private:
    void doAddObject(Object&& obj);
private:
    std::function<PreDisposeCallback> preDisposingObjectCallback_;
    std::vector<Object> disposingObjects_;
};
}

#endif // YADAW_SRC_UTIL_BATCHDISPOSER