#ifndef YADAW_SRC_AUDIO_BACKEND_AUDIOBACKENDSUPPORT
#define YADAW_SRC_AUDIO_BACKEND_AUDIOBACKENDSUPPORT

#include "audio/backend/AudioBackendBase.hpp"
#include "native/AudioBackendSupport.hpp"

namespace YADAW::Audio::Backend
{
template<AudioBackend>
constexpr bool isBackendSupported = false;

template<>
constexpr bool isBackendSupported<AudioBackend::Off> = true;

template<>
constexpr bool isBackendSupported<AudioBackend::AudioGraph> =
    YADAW::Native::isAudioBackendSupported<AudioBackend::AudioGraph>;

template<>
constexpr bool isBackendSupported<AudioBackend::ALSA> =
    YADAW::Native::isAudioBackendSupported<AudioBackend::ALSA>;

template<>
constexpr bool isBackendSupported<AudioBackend::WASAPIExclusive> =
    YADAW::Native::isAudioBackendSupported<AudioBackend::WASAPIExclusive>;
}

#endif // YADAW_SRC_AUDIO_BACKEND_AUDIOBACKENDSUPPORT
