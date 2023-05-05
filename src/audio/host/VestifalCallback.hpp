#ifndef YADAW_SRC_AUDIO_HOST_VESTIFALCALLBACK
#define YADAW_SRC_AUDIO_HOST_VESTIFALCALLBACK

#include "audio/plugin/vestifal/Vestifal.h"

#include <cstdint>

namespace YADAW::Audio::Host
{
std::intptr_t vestifalHostCallback(AEffect* effect, std::int32_t opcode, std::int32_t input, std::intptr_t opt, void* ptr, float value);

// When creating a plugin from shell, call this to pass the unique ID to the
// callback.
// The unique ID is thread_local, meaning that every thread has one instance of
// unique ID. This makes creating multiple plugins *scale*.
void setUniquePluginIdOnCurrentThread(std::int32_t uniqueId);

void setUniquePluginShouldBeZeroOnCurrentThread(bool value);
}

#endif //YADAW_SRC_AUDIO_HOST_VESTIFALCALLBACK
