#ifndef YADAW_SRC_UTIL_QSTRINGUTIL
#define YADAW_SRC_UTIL_QSTRINGUTIL

#include <QString>

namespace YADAW::Util
{
inline const char16_t* u16DataFromQString(const QString& string)
{
    return reinterpret_cast<const char16_t*>(string.constData());
}
}

#endif // YADAW_SRC_UTIL_QSTRINGUTIL
