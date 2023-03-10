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

AudioGraphBackend::Impl::DeviceInput::DeviceInput():
    deviceInputNode_(nullptr), frameOutputNode_(nullptr), audioBuffer_(nullptr) {}

AudioGraphBackend::Impl::DeviceInput::DeviceInput(AudioDeviceInputNode&& deviceInputNode, AudioFrameOutputNode&& frameOutputNode) :
    deviceInputNode_(std::move(deviceInputNode)), frameOutputNode_(std::move(frameOutputNode)), audioBuffer_(nullptr)
{
    deviceInputNode_.AddOutgoingConnection(frameOutputNode_);
}

AudioGraphBackend::Impl::Impl():
    audioInputDeviceInformationCollection_(DeviceInformation::FindAllAsync(DeviceClass::AudioCapture).get()),
    audioOutputDeviceInformationCollection_(DeviceInformation::FindAllAsync(DeviceClass::AudioRender).get()),
    audioGraph_(nullptr),
    audioDeviceOutputNode_(nullptr),
    audioFrameInputNode_(nullptr),
    deviceInputNodes_(),
    inputAudioBuffer_(),
    eventToken_()
{
    deviceInputNodes_.reserve(audioInputDeviceInformationCollection_.Size());
    inputAudioBuffer_.reserve(audioInputDeviceInformationCollection_.Size());
}

AudioGraphBackend::Impl::~Impl()
{
}

int AudioGraphBackend::Impl::audioInputDeviceCount() const
{
    return audioInputDeviceInformationCollection_.Size();
}

int AudioGraphBackend::Impl::audioOutputDeviceCount() const
{
    return audioOutputDeviceInformationCollection_.Size();
}

DeviceInformation AudioGraphBackend::Impl::audioInputDeviceAt(int index) const
{
    return audioInputDeviceInformationCollection_.GetAt(index);
}

DeviceInformation AudioGraphBackend::Impl::audioOutputDeviceAt(int index) const
{
    return audioOutputDeviceInformationCollection_.GetAt(index);
}

bool AudioGraphBackend::Impl::createAudioGraph()
{
    AudioGraphSettings settings(AudioRenderCategory::Media);
    settings.DesiredRenderDeviceAudioProcessing(AudioProcessing::Raw);
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
    settings.DesiredRenderDeviceAudioProcessing(AudioProcessing::Raw);
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

int AudioGraphBackend::Impl::enableDeviceInput(const DeviceInformation& audioInputDevice)
{
    auto result = audioGraph_.CreateDeviceInputNodeAsync(MediaCategory::Media, audioGraph_.EncodingProperties(), audioInputDevice).get();
    if(auto status = result.Status(); status != decltype(result.Status())::Success)
    {
        return -1;
    }
    deviceInputNodes_.emplace_back(result.DeviceInputNode(),
        audioGraph_.CreateFrameOutputNode());
    inputAudioBuffer_.emplace_back();
    return deviceInputNodes_.size() - 1;
}

bool AudioGraphBackend::Impl::disableDeviceInput(int deviceInputIndex)
{
    if(deviceInputIndex < deviceInputNodes_.size())
    {
        inputAudioBuffer_.erase(inputAudioBuffer_.begin() + deviceInputIndex);
        deviceInputNodes_.erase(deviceInputNodes_.begin() + deviceInputIndex);
        return true;
    }
    return false;
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

int AudioGraphBackend::Impl::currentInputDeviceCount() const
{
    return deviceInputNodes_.size();
}

DeviceInformation AudioGraphBackend::Impl::currentInputDeviceAt(int index) const
{
    return deviceInputNodes_[index].deviceInputNode_.Device();
}

void AudioGraphBackend::Impl::destroyAudioGraph()
{
    audioDeviceOutputNode_ = { nullptr };
    audioFrameInputNode_ = { nullptr };
    deviceInputNodes_.clear();
    audioGraph_ = { nullptr };
}

void AudioGraphBackend::Impl::start(AudioGraphBackend::AudioCallbackType* callback)
{
    eventToken_ = audioFrameInputNode_.QuantumStarted(
        [callback, this](const AudioFrameInputNode& sender, const FrameInputNodeQuantumStartedEventArgs& args)
        {
            if(auto requiredSamples = args.RequiredSamples(); requiredSamples != 0)
            {
                auto properties = sender.EncodingProperties();
                auto bufferSize = requiredSamples * properties.BitsPerSample() * properties.ChannelCount();
                for(int i = 0; i < inputAudioBuffer_.size(); ++i)
                {
                    auto& input = deviceInputNodes_[i];
                    if(auto inputFrame = input.frameOutputNode_.GetFrame();
                        inputFrame)
                    {
                        // auto discontinuous = inputFrame.IsDiscontinuous();
                        input.audioBuffer_ = inputFrame.LockBuffer(AudioBufferAccessMode::Read);
                        auto ref = input.audioBuffer_.CreateReference();
                        auto byteAccess = ref.as<IMemoryBufferByteAccess>();
                        std::uint8_t* dataInBytes = nullptr;
                        std::uint32_t capacityInBytes = 0;
                        if(byteAccess->GetBuffer(&dataInBytes, &capacityInBytes) == S_OK)
                        {
                            inputAudioBuffer_[i] = {
                                dataInBytes,
                                static_cast<int>(input.frameOutputNode_.EncodingProperties().ChannelCount()),
                                requiredSamples,
                                SampleFormat::Float32
                            };
                        }
                    }
                }
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
                        callback(inputAudioBuffer_.size(), inputAudioBuffer_.data(), 1, &interleaveAudioBuffer);
                    }
                }
                sender.AddFrame(frame);
                for(int i = 0; i < inputAudioBuffer_.size(); ++i)
                {
                    deviceInputNodes_[i].audioBuffer_ = nullptr;
                }
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

int AudioGraphBackend::Impl::bufferSizeInFrames() const
{
    return audioGraph_.SamplesPerQuantum();
}

int AudioGraphBackend::Impl::latencyInSamples() const
{
    return audioGraph_.LatencyInSamples();
}

std::uint32_t YADAW::Audio::Backend::AudioGraphBackend::Impl::sampleRate() const
{
    return audioGraph_.EncodingProperties().SampleRate();
}
}
