#ifndef YADAW_SRC_NATIVE_VESTIFALNATIVE
#define YADAW_SRC_NATIVE_VESTIFALNATIVE

#include "audio/plugin/VestifalPlugin.hpp"
#include "audio/plugin/vestifal/Vestifal.h"
#include "native/Library.hpp"

#include <QLatin1StringView>
#include <QString>

namespace YADAW::Native
{
extern QLatin1StringView vestifalExt;
extern QString vestifalPattern;
}

#endif // YADAW_SRC_NATIVE_VESTIFALNATIVE
