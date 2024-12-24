#if _WIN32

#include "AudioGraphBackendImpl.hpp"
#include "audio/backend/AudioGraphBackend.hpp"
#include "util/IntegerRange.hpp"

#include <cstdint>

#include <guiddef.h>

#include "native/win/winrt/Forward.hpp"
#include "native/win/winrt/Async.hpp"

#include <../include/winrt/Windows.Foundation.h>
#include <../include/winrt/Windows.Foundation.Collections.h>
#include <../include/winrt/Windows.Devices.Enumeration.h>
#include <../include/winrt/Windows.Media.h>
#include <../include/winrt/Windows.Media.Audio.h>
#include <../include/winrt/Windows.Media.Devices.h>
#include <../include/winrt/Windows.Media.MediaProperties.h>
#include <../include/winrt/Windows.Media.Render.h>

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

void setChannelCount(AudioEncodingProperties& properties, std::uint32_t channelCount)
{
    properties.ChannelCount(channelCount);
    properties.Bitrate(properties.SampleRate() * channelCount * 32/*bit depth*/);
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

std::uint32_t AudioGraphBackend::Impl::sampleRate() const
{
    return audioGraph_.EncodingProperties().SampleRate();
}

winrt::hstring AudioGraphBackend::Impl::defaultAudioInputDeviceId() const
{
    return MediaDevice::GetDefaultAudioCaptureId(AudioDeviceRole::Default);
}

winrt::hstring AudioGraphBackend::Impl::defaultAudioOutputDeviceId() const
{
    return MediaDevice::GetDefaultAudioRenderId(AudioDeviceRole::Default);
}

AudioGraphBackend::ErrorCode AudioGraphBackend::Impl::createAudioGraph(std::uint32_t sampleRate)
{
    auto settings = createAudioGraphSettings();
    if(sampleRate != 0)
    {
        settings.EncodingProperties().SampleRate(sampleRate);
    }
    auto createGraphResult = asyncResult(AudioGraph::CreateAsync(settings));
    if(createGraphResult.Status() != decltype(createGraphResult.Status())::Success)
    {
        return createGraphResult.ExtendedError().value;
    }
    audioGraph_ = createGraphResult.Graph();
    auto createOutputResult = asyncResult(audioGraph_.CreateDeviceOutputNodeAsync());
    if(createOutputResult.Status() != decltype(createOutputResult.Status())::Success)
    {
        return createOutputResult.ExtendedError().value;
    }
    audioDeviceOutputNode_ = createOutputResult.DeviceOutputNode();
    audioFrameInputNode_ = audioGraph_.CreateFrameInputNode();
    audioFrameInputNode_.AddOutgoingConnection(audioDeviceOutputNode_);
    return S_OK;
}

AudioGraphBackend::ErrorCode AudioGraphBackend::Impl::createAudioGraph(
    const DeviceInformation& audioOutputDevice,
    std::uint32_t sampleRate)
{
    auto settings = createAudioGraphSettings();
    settings.PrimaryRenderDevice(audioOutputDevice);
    if(sampleRate != 0)
    {
        settings.EncodingProperties().SampleRate(sampleRate);
    }
    auto createGraphResult = asyncResult(AudioGraph::CreateAsync(settings));
    if(createGraphResult.Status() != decltype(createGraphResult.Status())::Success)
    {
        return createGraphResult.ExtendedError().value;
    }
    audioGraph_ = createGraphResult.Graph();
    auto createOutputResult = asyncResult(audioGraph_.CreateDeviceOutputNodeAsync());
    if(createOutputResult.Status() != decltype(createOutputResult.Status())::Success)
    {
        return createOutputResult.ExtendedError().value;
    }
    audioDeviceOutputNode_ = createOutputResult.DeviceOutputNode();
    audioFrameInputNode_ = audioGraph_.CreateFrameInputNode();
    audioFrameInputNode_.AddOutgoingConnection(audioDeviceOutputNode_);
    return ERROR_SUCCESS;
}

std::optional<std::uint32_t> AudioGraphBackend::Impl::findAudioInputDeviceById(const winrt::hstring& id)
{
    auto it = std::find_if(
        audioInputDeviceInformationCollection_.begin(),
        audioInputDeviceInformationCollection_.end(),
        [&id](const DeviceInformation& deviceInformation)
        {
            return deviceInformation.Id() == id;
        }
    );
    if(it != audioInputDeviceInformationCollection_.end())
    {
        return {
            static_cast<std::uint32_t>(
                it - audioInputDeviceInformationCollection_.begin()
            )
        };
    }
    return std::nullopt;
}

bool AudioGraphBackend::Impl::isDeviceInputActivated(std::uint32_t deviceInputIndex) const
{
    if(deviceInputIndex < audioInputDeviceCount())
    {
        return static_cast<bool>(deviceInputNodes_[deviceInputIndex].deviceInputNode_);
    }
    return false;
}

AudioGraphBackend::ErrorCode AudioGraphBackend::Impl::activateDeviceInput(
    std::uint32_t deviceInputIndex, bool enabled)
{
    AudioGraphBackend::ErrorCode ret = E_INVALIDARG;
    if(deviceInputIndex < audioInputDeviceCount())
    {
        if(enabled)
        {
            if(deviceInputNodes_[deviceInputIndex].audioBuffer_)
            {
                return S_OK;
            }
            auto encodingProperties = audioGraph_.EncodingProperties();
            for(std::uint32_t i = 2; i > 0; --i)
            {
                setChannelCount(encodingProperties, i);
                auto result = asyncResult(
                    audioGraph_.CreateDeviceInputNodeAsync(
                        MediaCategory::Media,
                        encodingProperties,
                        audioInputDeviceInformationCollection_.GetAt(deviceInputIndex)
                    )
                );
                auto status = result.Status();
                ret = result.ExtendedError().value;
                if(status == decltype(status)::Success)
                {
                    deviceInputNodes_[deviceInputIndex] = {
                        result.DeviceInputNode(),
                        audioGraph_.CreateFrameOutputNode(encodingProperties)
                };
                return result.ExtendedError().value;
                }
            }
        }
        else
        {
            if(deviceInputNodes_[deviceInputIndex].deviceInputNode_)
            {
                deviceInputNodes_[deviceInputIndex] = {};
            }
            return S_OK;
        }
    }
    return ret;
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
                FOR_RANGE0(i, inputAudioBuffer_.size())
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
                            auto bufferData = ref.data();
                            inputAudioBuffer_[i] = AudioGraphBackend::InterleaveAudioBuffer {
                                bufferData,
                                static_cast<int>(input.frameOutputNode_.EncodingProperties().ChannelCount()),
                                requiredSamples
                            };
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
                    InterleaveAudioBuffer interleaveAudioBuffer
                    {
                        ref.data(),
                        static_cast<int>(properties.ChannelCount()),
                        requiredSamples
                    };
                    std::memset(ref.data(), 0, ref.Capacity());
                    callback(inputAudioBuffer_.size(), inputAudioBuffer_.data(), 1, &interleaveAudioBuffer);
                }
                sender.AddFrame(frame);
                FOR_RANGE0(i, inputAudioBuffer_.size())
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

#endif