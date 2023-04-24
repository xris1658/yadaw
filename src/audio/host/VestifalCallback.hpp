#ifndef YADAW_SRC_AUDIO_HOST_VESTIFALCALLBACK
#define YADAW_SRC_AUDIO_HOST_VESTIFALCALLBACK

#include "audio/plugin/vestifal/Vestifal.h"

#include <cstdint>

namespace YADAW::Audio::Host
{
std::intptr_t vestifalHostCallback(AEffect* effect, std::int32_t opcode, std::int32_t input, std::intptr_t opt, void* ptr, float value);

void setUniquePluginId(std::int32_t uniqueId);
}

#endif //YADAW_SRC_AUDIO_HOST_VESTIFALCALLBACK
