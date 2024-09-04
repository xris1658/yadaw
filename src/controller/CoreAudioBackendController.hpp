#ifndef YADAW_SRC_CONTROLLER_COREAUDIOBACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_COREAUDIOBACKENDCONTROLLER

#if __APPLE__

#include "audio/backend/CoreAudioBusConfiguration.hpp"

namespace YADAW::Controller
{
YADAW::Audio::Backend::CoreAudioBusConfiguration& appAudioBusConfiguration();
}

#endif

#endif // YADAW_SRC_CONTROLLER_COREAUDIOBACKENDCONTROLLER