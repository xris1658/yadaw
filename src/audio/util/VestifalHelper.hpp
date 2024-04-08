#ifndef YADAW_SRC_AUDIO_UTIL_VESTIFALHELPER
#define YADAW_SRC_AUDIO_UTIL_VESTIFALHELPER

#include "audio/plugin/VestifalPlugin.hpp"
#include "audio/plugin/vestifal/Vestifal.h"
#include "native/Library.hpp"

#include <cstdint>

inline intptr_t runDispatcher(AEffect* effect, std::int32_t opcode, std::int32_t input = 0, std::intptr_t opt = 0, void* ptr = nullptr, float value = 0.0f)
{
    return effect->dispatcher(effect, opcode, input, opt, ptr, value);
}

namespace YADAW::Audio::Util
{
VestifalEntry vestifalEntryFromLibrary(const YADAW::Native::Library& library);

YADAW::Audio::Plugin::VestifalPlugin createVestifalFromLibrary(
    const YADAW::Native::Library& library, std::int32_t uid = 0);

QString getVersionString(std::uint32_t version);
}

#endif // YADAW_SRC_AUDIO_UTIL_VESTIFALHELPER
