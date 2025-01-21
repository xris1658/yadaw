#ifndef YADAW_SRC_AUDIO_BACKEND_COREAUDIOBACKEND
#define YADAW_SRC_AUDIO_BACKEND_COREAUDIOBACKEND

#if __APPLE__

#include <../../System/Library/Frameworks/CoreAudio.framework/Headers/AudioHardware.h>

#include <QString>

#include <cstdint>

namespace YADAW::Audio::Backend
{
class CoreAudioBackend
{
public:
    struct DeviceInfo
    {
        AudioDeviceID id;
        QString name;
        DeviceInfo(): id(0), name() {}
        DeviceInfo(AudioDeviceID id, const QString& name): id(id), name(name) {}
        DeviceInfo(const QString&& name): id(id), name(std::move(name)) {}
    };
private:
    class Impl;
public:
    static std::uint32_t audioInputDeviceCount();
    static std::uint32_t audioOutputDeviceCount();
    static std::optional<DeviceInfo> audioInputDeviceAt(std::uint32_t index);
    static std::optional<DeviceInfo> audioOutputDeviceAt(std::uint32_t index);
    static AudioDeviceID defaultInputDevice();
    static AudioDeviceID defaultOutputDevice();
};
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_COREAUDIOBACKEND