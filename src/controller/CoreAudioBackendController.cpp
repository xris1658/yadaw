#if __APPLE__

#include "CoreAudioBackendController.hpp"

namespace YADAW::Controller
{
YADAW::Audio::Backend::CoreAudioBusConfiguration& appAudioBusConfiguration()
{
    static YADAW::Audio::Backend::CoreAudioBusConfiguration ret;
    return ret;
}
}

#endif