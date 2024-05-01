#ifndef YADAW_SRC_AUDIO_BACKEND_AUDIOBACKENDBASE
#define YADAW_SRC_AUDIO_BACKEND_AUDIOBACKENDBASE

namespace YADAW::Audio::Backend
{
enum AudioBackend
{
    Off,
    AudioGraph,
    ALSA,
    WASAPIExclusive
};
}

#endif // YADAW_SRC_AUDIO_BACKEND_AUDIOBACKENDBASE
