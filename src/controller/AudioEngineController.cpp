#include "AudioEngineController.hpp"

namespace YADAW::Controller
{

YADAW::Audio::Mixer::Mixer& appMixer()
{
    static YADAW::Audio::Mixer::Mixer ret;
    return ret;
}
}