#ifndef YADAW_SRC_NATIVE_WIN_AUDIOGRAPHBACKENDIMPL
#define YADAW_SRC_NATIVE_WIN_AUDIOGRAPHBACKENDIMPL

#if _WIN32

#include "audio/backend/AudioGraphBackend.hpp"
#include "native/Native.hpp"

#include <guiddef.h>

#include <Unknwn.h>

#include <cstdint>

#include "native/win/winrt/Forward.hpp"

#include <../include/winrt/Windows.Foundation.h>
#include <../include/winrt/Windows.Foundation.Collections.h>
#include <../include/winrt/Windows.Media.h>
#include <../include/winrt/Windows.Media.Audio.h>
#include <../include/winrt/Windows.Media.MediaProperties.h>
#include <../include/winrt/Windows.Media.Render.h>
#include <../include/winrt/Windows.Devices.Enumeration.h>


#include <vector>

namespace YADAW::Audio::Backend
{
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
    struct DeviceInput
    {
        DeviceInput();
        DeviceInput(AudioDeviceInputNode&& deviceInputNode, AudioFrameOutputNode&& frameOutputNode);
        DeviceInput(const DeviceInput&) = delete;
        DeviceInput& operator=(const DeviceInput&) = delete;
        DeviceInput(DeviceInput&& rhs) noexcept;
        DeviceInput& operator=(DeviceInput&& rhs) noexcept;
        AudioDeviceInputNode deviceInputNode_;
        AudioFrameOutputNode frameOutputNode_;
        AudioBuffer audioBuffer_;
    };
public:
    Impl();
    Impl(const Impl&) = delete;
    Impl(Impl&&) = delete;
    ~Impl();
public:
    std::uint32_t audioInputDeviceCount() const;
    std::uint32_t audioOutputDeviceCount() const;
    DeviceInformation audioInputDeviceAt(std::uint32_t index) const;
    DeviceInformation audioOutputDeviceAt(std::uint32_t index) const;
    winrt::hstring defaultAudioInputDeviceId() const;
    winrt::hstring defaultAudioOutputDeviceId() const;
    AudioGraphBackend::ErrorCode createAudioGraph(std::uint32_t sampleRate = 0);
    AudioGraphBackend::ErrorCode createAudioGraph(const DeviceInformation& audioOutputDevice, std::uint32_t sampleRate = 0);
    std::optional<std::uint32_t> findAudioInputDeviceById(const winrt::hstring& id);
    bool isDeviceInputActivated(std::uint32_t deviceInputIndex) const;
    AudioGraphBackend::ErrorCode activateDeviceInput(std::uint32_t deviceInputIndex, bool enabled);
    // This function might fail, in which case returns an invalid DeviceInformation
    DeviceInformation currentOutputDevice() const;
    void destroyAudioGraph();
    void start(AudioGraphBackend::AudioCallbackType* callback);
    void stop();
    int bufferSizeInFrames() const;
    int latencyInSamples() const;
    std::uint32_t sampleRate() const;
    std::uint32_t channelCount(bool isInput, std::uint32_t deviceIndex) const;
private:
    DeviceInformationCollection audioInputDeviceInformationCollection_;
    DeviceInformationCollection audioOutputDeviceInformationCollection_;
    AudioGraph audioGraph_;
    AudioDeviceOutputNode audioDeviceOutputNode_;
    AudioFrameInputNode audioFrameInputNode_;
    std::vector<DeviceInput> deviceInputNodes_;
    std::vector<InterleaveAudioBuffer> inputAudioBuffer_;
    event_token eventToken_;
};
}

#endif

#endif // YADAW_SRC_NATIVE_WIN_AUDIOGRAPHBACKENDIMPL
