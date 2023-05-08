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
    std::uint32_t defaultAudioInputDeviceIndex() const;
    std::uint32_t defaultAudioOutputDeviceIndex() const;
    bool createAudioGraph();
    bool createAudioGraph(const DeviceInformation& audioOutputDevice);
    bool isDeviceInputActivated(std::uint32_t deviceInputIndex) const;
    DeviceInputResult activateDeviceInput(std::uint32_t deviceInputIndex, bool enabled);
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

#endif //YADAW_SRC_NATIVE_AUDIOGRAPHBACKENDIMPL
