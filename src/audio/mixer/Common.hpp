#ifndef YADAW_SRC_AUDIO_MIXER_COMMON
#define YADAW_SRC_AUDIO_MIXER_COMMON

#include "util/AutoIncrementID.hpp"
#include "util/PolymorphicDeleter.hpp"

namespace YADAW::Audio::Mixer
{
using IDGen = YADAW::Util::AutoIncrementID;
using Context = YADAW::Util::PMRUniquePtr<void>;
}

#endif // YADAW_SRC_AUDIO_MIXER_COMMON