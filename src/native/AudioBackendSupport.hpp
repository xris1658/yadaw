#ifndef YADAW_SRC_NATIVE_AUDIOBACKENDSUPPORT
#define YADAW_SRC_NATIVE_AUDIOBACKENDSUPPORT

#include "audio/backend/AudioBackendBase.hpp"

namespace YADAW::Native
{
template<YADAW::Audio::Backend::AudioBackend> constexpr bool isAudioBackendSupported = false;

#if _WIN32
template<> constexpr bool isAudioBackendSupported<YADAW::Audio::Backend::AudioBackend::AudioGraph> = true;
template<> constexpr bool isAudioBackendSupported<YADAW::Audio::Backend::AudioBackend::WASAPIExclusive> = true;
#endif

#if __linux__
template<> constexpr bool isAudioBackendSupported<YADAW::Audio::Backend::AudioBackend::ALSA> = true;
#endif
}

#endif // YADAW_SRC_NATIVE_AUDIOBACKENDSUPPORT
