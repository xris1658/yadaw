#ifndef YADAW_SRC_AUDIO_UTIL_VESTIFALUTIL
#define YADAW_SRC_AUDIO_UTIL_VESTIFALUTIL

#include "audio/plugin/vestifal/Vestifal.h"

#include <cstdint>

inline intptr_t runDispatcher(AEffect* effect, std::int32_t opcode, std::int32_t input = 0, std::intptr_t opt = 0, void* ptr = nullptr, float value = 0.0f)
{
    return effect->dispatcher(effect, opcode, input, opt, ptr, value);
}

#endif //YADAW_SRC_AUDIO_UTIL_VESTIFALUTIL
