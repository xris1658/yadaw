#include "QStringFromHString.hpp"

namespace YADAW::Native
{
QString qStringFromHString(const winrt::hstring& string)
{
    return QString::fromUtf16(reinterpret_cast<const char16_t*>(string.data()));
}
}