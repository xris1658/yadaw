#include "AudioGraphBackendImpl.hpp"
#include "audio/backend/AudioGraphBackend.hpp"

#include <cstdint>

#include "native/win/winrt/Forward.hpp"
#include "native/win/winrt/Async.hpp"

#include <MemoryBuffer.h> // Windows::Foundation::IMemoryBufferByteAccess
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Audio.h>
#include <winrt/Windows.Media.Devices.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Render.h>
#include <winrt/Windows.Devices.Enumeration.h>

#if __cplusplus <= 201703L
#include <future>
#endif

using namespace ::Windows::Foundation;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Audio;
using namespace winrt::Windows::Media::Devices;
using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::MediaProperties;
using namespace winrt::Windows::Media::Render;
using namespace winrt::Windows::Devices::Enumeration;

AudioGraphSettings createAudioGraphSettings()
{
    AudioGraphSettings ret(AudioRenderCategory::Media);
    ret.DesiredRenderDeviceAudioProcessing(AudioProcessing::Raw);
    return ret;
}

namespace YADAW::Audio::Backend
{
AudioGraphBackend::Impl::DeviceInput::DeviceInput():
    deviceInputNode_(nullptr), frameOutputNode_(nullptr), audioBuffer_(nullptr) {}

AudioGraphBackend::Impl::DeviceInput::DeviceInput(AudioDeviceInputNode&& deviceInputNode, AudioFrameOutputNode&& frameOutputNode) :
    deviceInputNode_(std::move(deviceInputNode)), frameOutputNode_(std::move(frameOutputNode)), audioBuffer_(nullptr)
{
    deviceInputNode_.AddOutgoingConnection(frameOutputNode_);
}

AudioGraphBackend::Impl::DeviceInput::DeviceInput(DeviceInput&& rhs) noexcept = default;
AudioGraphBackend::Impl::DeviceInput& AudioGraphBackend::Impl::DeviceInput::operator=(DeviceInput&& rhs) noexcept = default;

AudioGraphBackend::Impl::Impl():
    audioInputDeviceInformationCollection_(nullptr),
    audioOutputDeviceInformationCollection_(nullptr),
    audioGraph_(nullptr),
    audioDeviceOutputNode_(nullptr),
    audioFrameInputNode_(nullptr),
    deviceInputNodes_(),
    inputAudioBuffer_(),
    eventToken_()
{
    // Suppress the `is_sta()` assert failure
    audioInputDeviceInformationCollection_ = asyncResult(DeviceInformation::FindAllAsync(DeviceClass::AudioCapture));
    audioOutputDeviceInformationCollection_ = asyncResult(DeviceInformation::FindAllAsync(DeviceClass::AudioRender));
    deviceInputNodes_.resize(audioInputDeviceInformationCollection_.Size());
    inputAudioBuffer_.resize(audioInputDeviceInformationCollection_.Size());
}

AudioGraphBackend::Impl::~Impl()
{
}

std::uint32_t AudioGraphBackend::Impl::audioInputDeviceCount() const
{
    return audioInputDeviceInformationCollection_.Size();
}

std::uint32_t AudioGraphBackend::Impl::audioOutputDeviceCount() const
{
    return audioOutputDeviceInformationCollection_.Size();
}

DeviceInformation AudioGraphBackend::Impl::audioInputDeviceAt(std::uint32_t index) const
{
    return audioInputDeviceInformationCollection_.GetAt(index);
}

DeviceInformation AudioGraphBackend::Impl::audioOutputDeviceAt(std::uint32_t index) const
{
    return audioOutputDeviceInformationCollection_.GetAt(index);
}

winrt::hstring AudioGraphBackend::Impl::defaultAudioInputDeviceId() const
{
    return MediaDevice::GetDefaultAudioCaptureId(AudioDeviceRole::Default);
}

winrt::hstring AudioGraphBackend::Impl::defaultAudioOutputDeviceId() const
{
    return MediaDevice::GetDefaultAudioRenderId(AudioDeviceRole::Default);
}

bool AudioGraphBackend::Impl::createAudioGraph()
{
    auto settings = createAudioGraphSettings();
    auto createGraphResult = asyncResult(AudioGraph::CreateAsync(settings));
    if(createGraphResult.Status() != decltype(createGraphResult.Status())::Success)
    {
        return false;
    }
    audioGraph_ = createGraphResult.Graph();
    auto createOutputResult = asyncResult(audioGraph_.CreateDeviceOutputNodeAsync());
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
    auto settings = createAudioGraphSettings();
    settings.PrimaryRenderDevice(audioOutputDevice);
    auto createGraphResult = asyncResult(AudioGraph::CreateAsync(settings));
    if(createGraphResult.Status() != decltype(createGraphResult.Status())::Success)
    {
        return false;
    }
    audioGraph_ = createGraphResult.Graph();
    auto createOutputResult = asyncResult(audioGraph_.CreateDeviceOutputNodeAsync());
    if(createOutputResult.Status() != decltype(createOutputResult.Status())::Success)
    {
        return false;
    }
    audioDeviceOutputNode_ = createOutputResult.DeviceOutputNode();
    audioFrameInputNode_ = audioGraph_.CreateFrameInputNode();
    audioFrameInputNode_.AddOutgoingConnection(audioDeviceOutputNode_);
    return true;
}

bool AudioGraphBackend::Impl::isDeviceInputActivated(std::uint32_t deviceInputIndex) const
{
    if(deviceInputIndex < audioInputDeviceCount())
    {
        return static_cast<bool>(deviceInputNodes_[deviceInputIndex].deviceInputNode_);
    }
    return false;
}

AudioGraphBackend::DeviceInputResult
AudioGraphBackend::Impl::activateDeviceInput(std::uint32_t deviceInputIndex, bool enabled)
{
    if(deviceInputIndex < audioInputDeviceCount())
    {
        if(enabled)
        {
            if(deviceInputNodes_[deviceInputIndex].audioBuffer_)
            {
                return DeviceInputResult::AlreadyDone;
            }
            auto result = asyncResult(audioGraph_.CreateDeviceInputNodeAsync(
                MediaCategory::Media, audioGraph_.EncodingProperties(),
                audioInputDeviceInformationCollection_.GetAt(deviceInputIndex)
            ));
            if(auto status = result.Status(); status == decltype(status)::Success)
            {
                deviceInputNodes_[deviceInputIndex] = {result.DeviceInputNode(), audioGraph_.CreateFrameOutputNode()};
                return DeviceInputResult::Success;
            }
        }
        else
        {
            if(deviceInputNodes_[deviceInputIndex].deviceInputNode_)
            {
                deviceInputNodes_[deviceInputIndex] = {};
                return DeviceInputResult::Success;
            }
            return DeviceInputResult::AlreadyDone;
        }
    }
    return DeviceInputResult::Failed;
}

DeviceInformation AudioGraphBackend::Impl::currentOutputDevice() const
{
    auto ret = audioGraph_.PrimaryRenderDevice();
    if(ret)
    {
        return ret;
    }
    return asyncResult(DeviceInformation::CreateFromIdAsync(defaultAudioOutputDeviceId()));
}

void AudioGraphBackend::Impl::destroyAudioGraph()
{
    audioDeviceOutputNode_ = { nullptr };
    audioFrameInputNode_ = { nullptr };
    for(auto& deviceInputNode: deviceInputNodes_)
    {
        deviceInputNode = {};
    }
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
                    if(input.deviceInputNode_)
                    {
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
                                inputAudioBuffer_[i] = AudioGraphBackend::InterleaveAudioBuffer {
                                    /*.data = */dataInBytes,
                                    /*.channelCount = */static_cast<int>(input.frameOutputNode_.EncodingProperties().ChannelCount()),
                                    /*.frameCount = */requiredSamples,
                                    /*.sampleFormat = */SampleFormat::Float32
                                };
                            }
                        }
                    }
                    else
                    {
                        inputAudioBuffer_[i] = {};
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

std::uint32_t AudioGraphBackend::Impl::sampleRate() const
{
    return audioGraph_.EncodingProperties().SampleRate();
}

std::uint32_t AudioGraphBackend::Impl::channelCount(bool isInput, std::uint32_t deviceIndex) const
{
    if(isInput)
    {
        if(deviceIndex < deviceInputNodes_.size() && deviceInputNodes_[deviceIndex].deviceInputNode_)
        {
            return deviceInputNodes_[deviceIndex].deviceInputNode_.EncodingProperties().ChannelCount();
        }
    }
    else
    {
        if(deviceIndex < audioOutputDeviceCount())
        {
            const auto& currentId = currentOutputDevice().Id();
            if(currentId == audioOutputDeviceInformationCollection_.GetAt(deviceIndex).Id())
            {
                return audioDeviceOutputNode_.EncodingProperties().ChannelCount();
            }
        }
    }
    return 0;
}
}
