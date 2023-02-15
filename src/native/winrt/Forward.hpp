#ifndef YADAW_SRC_NATIVE_WINRT_FORWARD
#define YADAW_SRC_NATIVE_WINRT_FORWARD

#include <winrt/base.h>

namespace winrt::impl
{
template<typename Async> auto wait_for(const Async& async, const Windows::Foundation::TimeSpan& timeout);
}


#endif //YADAW_SRC_NATIVE_WINRT_FORWARD
