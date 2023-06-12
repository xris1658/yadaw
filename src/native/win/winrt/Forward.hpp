#ifndef YADAW_SRC_NATIVE_WIN_WINRT_FORWARD
#define YADAW_SRC_NATIVE_WIN_WINRT_FORWARD

#if(WIN32)

#include <winrt/base.h>

namespace winrt::impl
{
template<typename Async> auto wait_for(const Async& async, const Windows::Foundation::TimeSpan& timeout);
}

#endif

#endif // YADAW_SRC_NATIVE_WIN_WINRT_FORWARD
