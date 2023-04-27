#ifndef YADAW_SRC_NATIVE_VESTIFALNATIVE
#define YADAW_SRC_NATIVE_VESTIFALNATIVE

#include "audio/plugin/VestifalPlugin.hpp"
#include "native/Library.hpp"

namespace YADAW::Native
{
YADAW::Audio::Plugin::VestifalPlugin createVestifalFromLibrary(Library& library,
    std::int32_t uid = 0);
}

#endif //YADAW_SRC_NATIVE_VESTIFALNATIVE
