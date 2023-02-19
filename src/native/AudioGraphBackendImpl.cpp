#include "AudioGraphBackendImpl.hpp"
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
using namespace ::Windows::Foundation;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Audio;
using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::MediaProperties;
using namespace winrt::Windows::Media::Render;
using namespace winrt::Windows::Devices::Enumeration;

AudioGraphBackend::Impl::Impl():
    audioInputDeviceInformationCollection_(DeviceInformation::FindAllAsync(DeviceClass::AudioCapture).get()),
    audioOutputDeviceInformationCollection_(DeviceInformation::FindAllAsync(DeviceClass::AudioRender).get()),
    audioGraph_(nullptr),
    audioDeviceOutputNode_(nullptr),
    audioFrameInputNode_(nullptr)
{
}

AudioGraphBackend::Impl::~Impl()
{
    // TODO
}

int AudioGraphBackend::Impl::audioOutputDeviceCount() const
{
    return audioOutputDeviceInformationCollection_.Size();
}

int AudioGraphBackend::Impl::bufferSizeInFrames() const
{
    return audioGraph_.SamplesPerQuantum();
}

int AudioGraphBackend::Impl::latencyInSamples() const
{
    return audioGraph_.LatencyInSamples();
}

DeviceInformation AudioGraphBackend::Impl::audioOutputDeviceAt(int index) const
{
    return audioOutputDeviceInformationCollection_.GetAt(index);
}

bool AudioGraphBackend::Impl::createAudioGraph()
{
    AudioGraphSettings settings(AudioRenderCategory::Media);
    auto encodingProperties = settings.EncodingProperties();
    settings.EncodingProperties(encodingProperties);
    auto createGraphResult = AudioGraph::CreateAsync(settings).get();
    if(createGraphResult.Status() != decltype(createGraphResult.Status())::Success)
    {
        return false;
    }
    audioGraph_ = createGraphResult.Graph();
    auto createOutputResult = audioGraph_.CreateDeviceOutputNodeAsync().get();
    if(createOutputResult.Status() != decltype(createOutputResult.Status())::Success)
    {
        return false;
    }
    audioDeviceOutputNode_ = createOutputResult.DeviceOutputNode();
    audioFrameInputNode_ = audioGraph_.CreateFrameInputNode();
    audioFrameInputNode_.AddOutgoingConnection(audioDeviceOutputNode_);
    return true;
}

bool AudioGraphBackend::Impl::createAudioGraph(const DeviceInformation& audioOutputDevice)
{
    AudioGraphSettings settings(AudioRenderCategory::Media);
    auto encodingProperties = settings.EncodingProperties();
    settings.EncodingProperties(encodingProperties);
    settings.PrimaryRenderDevice(audioOutputDevice);
    auto createGraphResult = AudioGraph::CreateAsync(settings).get();
    if(createGraphResult.Status() != decltype(createGraphResult.Status())::Success)
    {
        return false;
    }
    audioGraph_ = createGraphResult.Graph();
    auto createOutputResult = audioGraph_.CreateDeviceOutputNodeAsync().get();
    if(createOutputResult.Status() != decltype(createOutputResult.Status())::Success)
    {
        return false;
    }
    audioDeviceOutputNode_ = createOutputResult.DeviceOutputNode();
    audioFrameInputNode_ = audioGraph_.CreateFrameInputNode();
    audioFrameInputNode_.AddOutgoingConnection(audioDeviceOutputNode_);
    return true;
}

DeviceInformation AudioGraphBackend::Impl::currentOutputDevice() const
{
    auto&& ret1 = audioGraph_.PrimaryRenderDevice();
    if(ret1)
    {
        return ret1;
    }
    return audioDeviceOutputNode_.Device();
}

void AudioGraphBackend::Impl::destroyAudioGraph()
{
    audioGraph_ = AudioGraph(nullptr);
}

void AudioGraphBackend::Impl::start(AudioGraphBackend::AudioCallbackType* callback)
{
    eventToken_ = audioFrameInputNode_.QuantumStarted(
        [callback](const AudioFrameInputNode& sender, const FrameInputNodeQuantumStartedEventArgs& args)
        {
            if(auto requiredSamples = args.RequiredSamples(); requiredSamples != 0)
            {
                auto properties = sender.EncodingProperties();
                auto bufferSize = requiredSamples * properties.BitsPerSample() * properties.ChannelCount();
                AudioFrame frame(bufferSize >> 3);
                {
                    AudioBuffer buffer = frame.LockBuffer(AudioBufferAccessMode::Write);
                    IMemoryBufferReference ref = buffer.CreateReference();
                    auto byteAccess = ref.as<IMemoryBufferByteAccess>();
                    std::uint8_t* dataInBytes = nullptr;
                    std::uint32_t capacityInBytes = 0;
                    if(byteAccess->GetBuffer(&dataInBytes, &capacityInBytes) == S_OK)
                    {
                        InterleaveAudioBuffer interleaveAudioBuffer
                        {
                            dataInBytes,
                            static_cast<int>(properties.ChannelCount()),
                            requiredSamples,
                            SampleFormat::Float32
                        };
                        callback(0, nullptr, 1, &interleaveAudioBuffer);
                    }
                }
                sender.AddFrame(frame);
            }
        }
    );
    audioGraph_.Start();
}

void AudioGraphBackend::Impl::stop()
{
    audioGraph_.Stop();
    audioFrameInputNode_.QuantumStarted(eventToken_);
}

std::uint32_t YADAW::Audio::Backend::AudioGraphBackend::Impl::sampleRate() const
{
    return audioGraph_.EncodingProperties().SampleRate();
}
}
