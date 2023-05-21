#ifndef YADAW_SRC_NATIVE_AUDIOBACKEND
#define YADAW_SRC_NATIVE_AUDIOBACKEND

#include "controller/AudioBackendController.hpp"

namespace YADAW::Native
{
using namespace YADAW::Controller;

template<AudioBackend Backend> constexpr bool isAudioBackendSupported = false;

template<> constexpr bool isAudioBackendSupported<AudioBackend::Dummy> = true;

#if(WIN32)
template<> constexpr bool isAudioBackendSupported<AudioBackend::AudioGraph> = true;
#endif

#if(__linux__)
template<> constexpr bool isAudioBackendSupported<AudioBackend::ALSA> = true;
#endif
}

#endif //YADAW_SRC_NATIVE_AUDIOBACKEND
