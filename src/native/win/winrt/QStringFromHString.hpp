#ifndef YADAW_SRC_NATIVE_WIN_WINRT_QSTRINGFROMHSTRING
#define YADAW_SRC_NATIVE_WIN_WINRT_QSTRINGFROMHSTRING

#if _WIN32

#include <../include/winrt/base.h>

#include <QString>

namespace YADAW::Native
{
QString qStringFromHString(const winrt::hstring& string);
}

#endif

#endif // YADAW_SRC_NATIVE_WIN_WINRT_QSTRINGFROMHSTRING
