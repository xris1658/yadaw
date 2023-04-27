#ifndef YADAW_SRC_NATIVE_WINRT_ASYNC
#define YADAW_SRC_NATIVE_WINRT_ASYNC

#include <winrt/Windows.Foundation.h>

#include <future>

template<typename T>
inline T asyncResult(winrt::Windows::Foundation::IAsyncOperation<T> asyncOperation)
{
// #if __cplusplus > 201703L
//     co_return asyncOperation; // C2039
// #else
#ifdef NDEBUG
    return asyncOperation.get();
#else
    T ret {nullptr};
    std::async(
        std::launch::async, [&ret, &asyncOperation]()
        { ret = asyncOperation.get(); }
    ).get();
    return ret;
#endif
// #endif
}

#endif //YADAW_SRC_NATIVE_WINRT_ASYNC
