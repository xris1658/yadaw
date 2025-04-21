#ifndef YADAW_SRC_UTIL_BATCHDISPOSER
#define YADAW_SRC_UTIL_BATCHDISPOSER

#include "PolymorphicDeleter.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace YADAW::Util
{
class BatchUpdater
{
public:
    using Object = YADAW::Util::PMRUniquePtr<void>;
    using UpdateCallback = void();
public:
    BatchUpdater(std::function<UpdateCallback>&& callback);
    ~BatchUpdater();
public:
    template<typename T, typename Deleter>
    void addObject(std::unique_ptr<T, Deleter>&& obj)
    {
        doAddObject(YADAW::Util::createPMRUniquePtr(std::move(obj)));
    }
    void addNull();
    void commit();
private:
    void doAddObject(Object&& obj);
private:
    std::function<UpdateCallback> updateCallback_;
    std::vector<Object> disposingObjects_;
    bool hasNull_ = false;
};
}

#endif // YADAW_SRC_UTIL_BATCHDISPOSER