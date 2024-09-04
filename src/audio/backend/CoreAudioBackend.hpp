#ifndef YADAW_SRC_AUDIO_BACKEND_COREAUDIOBACKEND
#define YADAW_SRC_AUDIO_BACKEND_COREAUDIOBACKEND

#if __APPLE__

#include <../../System/Library/Frameworks/CoreAudio.framework/Headers/CoreAudio.h>

namespace YADAW::Audio::Backend
{
class CoreAudioBackend
{
private:
    class Impl;
};
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_COREAUDIOBACKEND