#include "QStringUtil.hpp"

namespace YADAW::Util
{
const char16_t* u16DataFromQString(const QString& string)
{
    return reinterpret_cast<const char16_t*>(string.constData());
}
}