#ifndef YADAW_SRC_NATIVE_MAC_COREAUDIOBACKENDIMPL
#define YADAW_SRC_NATIVE_MAC_COREAUDIOBACKENDIMPL

#if __APPLE__

#include "audio/backend/CoreAudioBackend.hpp"

#include <../../System/Library/Frameworks/CoreAudio.framework/Headers/AudioHardware.h>

namespace YADAW::Audio::Backend
{
class CoreAudioBackend::Impl
{
public:
    static std::uint32_t audioInputDeviceCount();
    static std::uint32_t audioOutputDeviceCount();
};
}

#endif

#endif //YADAW_SRC_NATIVE_MAC_COREAUDIOBACKENDIMPL
