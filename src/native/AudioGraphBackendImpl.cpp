#include "AudioGraphBackendImpl.hpp"
#include "audio/backend/AudioGraphBackend.hpp"

#include <Unknwn.h>

#include <combaseapi.h>
#include <mmdeviceapi.h>

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

#if __cplusplus <= 201703L
#include <future>
#endif

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

template<typename T>
inline T asyncResult(winrt::Windows::Foundation::IAsyncOperation<T> asyncOperation)
{
// #if __cplusplus > 201703L
//     co_return asyncOperation; // C2039
// #else
    #ifdef NDEBUG
    return asyncOperation.get();
#else
    T ret {nullptr};
    std::async(
        std::launch::async, [&ret, &asyncOperation]()
        { ret = asyncOperation.get(); }
    ).get();
    return ret;
#endif
// #endif
}

namespace YADAW::Audio::Backend
{
namespace Helper
{
class IMMDeviceEnumeratorCreator
{
private:
    IMMDeviceEnumeratorCreator()
    {
        if(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&deviceEnumerator_)) != S_OK)
        {
            deviceEnumerator_ = nullptr;
            throw std::runtime_error("");
        }
    }
public:
    static IMMDeviceEnumerator* deviceEnumerator()
    {
        try
        {
            static IMMDeviceEnumeratorCreator instance;
            return instance.deviceEnumerator_;
        }
        catch(...)
        {
            return nullptr;
        }
    }
public:
    IMMDeviceEnumerator* deviceEnumerator_ = nullptr;
};
}

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

std::uint32_t AudioGraphBackend::Impl::defaultAudioInputDeviceIndex() const
{
    if(auto deviceEnumerator = Helper::IMMDeviceEnumeratorCreator::deviceEnumerator();
        deviceEnumerator)
    {
        IMMDevice* endpoint = nullptr;
        if(deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture,
            ERole::eMultimedia, &endpoint) == S_OK)
        {
            LPWSTR deviceId = nullptr;
            if(endpoint->GetId(&deviceId) == S_OK)
            {
                // https://learn.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdevice-getid#remarks
                // FIXME: Use IMMDeviceEnumerator::GetDevice to retrieve device ID
                int id = -1;
                for(int i = 0; i < audioInputDeviceInformationCollection_.Size(); ++i)
                {
                    // \\?\SWD#MMDEVAPI#{0.0.0.00000000}.{4c2d97cc-7925-4292-9964-6376fd05ffee}#{e6327cad-dcec-4949-ae8a-991e976a79d2}
                    auto idForCompare = audioInputDeviceInformationCollection_.GetAt(i).Id();
                    if(std::equal(deviceId, deviceId + std::wcslen(deviceId), idForCompare.c_str() + 17))
                    {
                        id = i;
                        break;
                    }
                }
                CoTaskMemFree(deviceId);
                return id;
            }
        }
    }
    return -1;
}

std::uint32_t AudioGraphBackend::Impl::defaultAudioOutputDeviceIndex() const
{
    if(auto deviceEnumerator = Helper::IMMDeviceEnumeratorCreator::deviceEnumerator();
        deviceEnumerator)
    {
        IMMDevice* endpoint = nullptr;
        if(deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender,
            ERole::eMultimedia, &endpoint) == S_OK)
        {
            // {0.0.0.00000000}.{4c2d97cc-7925-4292-9964-6376fd05ffee}
            LPWSTR deviceId = nullptr;
            if(endpoint->GetId(&deviceId) == S_OK)
            {
                // https://learn.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdevice-getid#remarks
                // FIXME: Use IMMDeviceEnumerator::GetDevice to retrieve device ID
                int id = -1;
                for(int i = 0; i < audioOutputDeviceInformationCollection_.Size(); ++i)
                {
                    // \\?\SWD#MMDEVAPI#{0.0.0.00000000}.{4c2d97cc-7925-4292-9964-6376fd05ffee}#{e6327cad-dcec-4949-ae8a-991e976a79d2}
                    auto idForCompare = audioOutputDeviceInformationCollection_.GetAt(i).Id();
                    if(std::equal(deviceId, deviceId + std::wcslen(deviceId), idForCompare.c_str() + 17))
                    {
                        id = i;
                        break;
                    }
                }
                CoTaskMemFree(deviceId);
                return id;
            }
        }
    }
    return -1;
}

bool AudioGraphBackend::Impl::createAudioGraph()
{
    AudioGraphSettings settings(AudioRenderCategory::Media);
    settings.DesiredRenderDeviceAudioProcessing(AudioProcessing::Raw);
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
    AudioGraphSettings settings(AudioRenderCategory::Media);
    settings.DesiredRenderDeviceAudioProcessing(AudioProcessing::Raw);
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
    return audioInputDeviceInformationCollection_.GetAt(defaultAudioOutputDeviceIndex());
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

std::uint32_t YADAW::Audio::Backend::AudioGraphBackend::Impl::sampleRate() const
{
    return audioGraph_.EncodingProperties().SampleRate();
}
}
