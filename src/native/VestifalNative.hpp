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

VestifalEntry vestifalEntryFromLibrary(Library& library);

YADAW::Audio::Plugin::VestifalPlugin createVestifalFromLibrary(Library& library,
    std::int32_t uid = 0);
}

#endif //YADAW_SRC_NATIVE_VESTIFALNATIVE
