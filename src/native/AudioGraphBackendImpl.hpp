#ifndef YADAW_SRC_NATIVE_AUDIOGRAPHBACKENDIMPL
#define YADAW_SRC_NATIVE_AUDIOGRAPHBACKENDIMPL

#include "audio/backend/AudioGraphBackend.hpp"

#include <Unknwn.h>

#include <cstdint>

#include "native/winrt/Forward.hpp"

#include <MemoryBuffer.h> // Windows::Foundation::IMemoryBufferByteAccess
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Audio.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Render.h>
#include <winrt/Windows.Devices.Enumeration.h>

#include <map>

namespace YADAW::Audio::Backend
{
using namespace Windows::Foundation;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Audio;
using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::MediaProperties;
using namespace winrt::Windows::Media::Render;
using namespace winrt::Windows::Devices::Enumeration;

class AudioGraphBackend::Impl
{
public:
    Impl();
    Impl(const Impl&) = delete;
    Impl(Impl&&) = delete;
    ~Impl();
public:
    int audioOutputDeviceCount() const;
    DeviceInformation audioOutputDeviceAt(int index) const;
    bool createAudioGraph();
    bool createAudioGraph(const DeviceInformation& audioOutputDevice);
    // This function might fail, in which case returns a invalid DeviceInformation
    DeviceInformation currentOutputDevice() const;
    void destroyAudioGraph();
    void start(AudioGraphBackend::AudioCallbackType* callback);
    void stop();
public:
    std::uint32_t sampleRate() const;
private:
    DeviceInformationCollection audioInputDeviceInformationCollection_;
    DeviceInformationCollection audioOutputDeviceInformationCollection_;
    AudioGraph audioGraph_;
    AudioDeviceOutputNode audioDeviceOutputNode_;
    AudioFrameInputNode audioFrameInputNode_;
    event_token eventToken_;
};
}

#endif //YADAW_SRC_NATIVE_AUDIOGRAPHBACKENDIMPL
