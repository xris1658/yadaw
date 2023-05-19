#ifndef YADAW_SRC_NATIVE_WINRT_QSTRINGFROMHSTRING
#define YADAW_SRC_NATIVE_WINRT_QSTRINGFROMHSTRING

#if(WIN32)

#include <winrt/base.h>

#include <QString>

namespace YADAW::Native
{
QString qStringFromHString(const winrt::hstring& string);
}

#endif

#endif //YADAW_SRC_NATIVE_WINRT_QSTRINGFROMHSTRING
