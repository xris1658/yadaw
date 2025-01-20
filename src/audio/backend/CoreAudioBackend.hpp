#ifndef YADAW_SRC_AUDIO_BACKEND_COREAUDIOBACKEND
#define YADAW_SRC_AUDIO_BACKEND_COREAUDIOBACKEND

#if __APPLE__

#include <cstdint>

namespace YADAW::Audio::Backend
{
class CoreAudioBackend
{
private:
    class Impl;
public:
    static std::uint32_t audioInputDeviceCount();
    static std::uint32_t audioOutputDeviceCount();
};
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_COREAUDIOBACKEND