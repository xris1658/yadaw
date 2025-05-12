#ifndef YADAW_SRC_UTIL_IMPL_REFCOUNTERBASE
#define YADAW_SRC_UTIL_IMPL_REFCOUNTERBASE

#include <atomic>

namespace YADAW::Util
{
template<typename T>
class IntrusivePointer;

class IntrusiveRefCounter
{
    template<typename T>
    friend class IntrusivePointer;
protected:
    IntrusiveRefCounter();
public:
    IntrusiveRefCounter(const IntrusiveRefCounter&) = delete;
    IntrusiveRefCounter(IntrusiveRefCounter&&) = delete;
    virtual ~IntrusiveRefCounter();
protected:
    void addRef();
    void release();
private:
    std::atomic_uint64_t refCount_;
};
}

#endif // YADAW_SRC_UTIL_IMPL_REFCOUNTERBASE
