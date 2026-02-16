#ifndef YADAW_SRC_UTIL_STATEFULCOROUTINE
#define YADAW_SRC_UTIL_STATEFULCOROUTINE

#include <coroutine>

namespace YADAW::Util
{
template<typename ReturnType>
struct StatefulPromise;

using StateID = std::uint32_t;

template<typename ReturnType>
struct StatefulCoroutine: std::coroutine_handle<StatefulPromise<ReturnType>>
{
    using StateId = std::uint32_t;
    using promise_type = StatefulPromise<ReturnType>;
};

constexpr StateID InitialState  = 0;
constexpr StateID FinishedState = 0xFFFFFFFF;

template<typename ReturnType>
struct StatefulPromise
{
    StatefulCoroutine<ReturnType> get_return_object()
    {
        return {StatefulCoroutine<ReturnType>::from_promise(*this)};
    }
    std::suspend_never initial_suspend() noexcept
    {
        return {};
    }
    std::suspend_always final_suspend() noexcept
    {
        return {};
    }
    std::suspend_always yield_value(std::uint32_t stateId) noexcept
    {
        this->stateId = stateId;
        return {};
    }
    void return_value(ReturnType&& ret) { stateId = FinishedState; this->ret = std::forward<ReturnType>(ret); } // FIXME
    void unhandled_exception() {}
    ReturnType ret;
    StateID stateId = InitialState;
};

template<> struct StatefulPromise<void>
{
    StatefulCoroutine<void> get_return_object()
    {
        return {StatefulCoroutine<void>::from_promise(*this)};
    }
    std::suspend_never initial_suspend() noexcept
    {
        return {};
    }
    std::suspend_always final_suspend() noexcept
    {
        return {};
    }
    std::suspend_always yield_value(std::uint32_t stateId) noexcept
    {
        this->stateId = stateId;
        return {};
    }
    void return_void() { stateId = FinishedState; }
    void unhandled_exception() {}
    StateID stateId = InitialState;
};
}

#endif // YADAW_SRC_UTIL_STATEFULCOROUTINE