#include "RollbackableOperation.hpp"

namespace YADAW::Util
{
struct RollbackablePromise
{
    RollbackableOperation get_return_object()
    {
        return {RollbackableOperation::from_promise(*this)};
    }
    std::suspend_never initial_suspend() noexcept
    {
        return {};
    }
    std::suspend_always final_suspend() noexcept
    {
        return {};
    }
    std::suspend_always yield_value(bool& shouldCommit) noexcept
    {
        shouldCommit_ = std::reference_wrapper<bool>(shouldCommit);
        return {};
    }
    void return_void() {}
    void unhandled_exception() {}
    OptionalRef<bool> shouldCommit_ = std::nullopt;
};

void RollbackableOperation::resume(bool shouldCommit)
{
    if(auto optRefShouldCommit = promise().shouldCommit_)
    {
        optRefShouldCommit->get() = shouldCommit;
        this->operator()();
    }
}
}
