#if(WIN32)

#include "QStringFromHString.hpp"

namespace YADAW::Native
{
QString qStringFromHString(const winrt::hstring& string)
{
    return QString::fromWCharArray(string.data());
}
}

#endif