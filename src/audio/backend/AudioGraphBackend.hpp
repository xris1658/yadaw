#ifndef YADAW_SRC_AUDIO_BACKEND_AUDIOGRAPHBACKEND
#define YADAW_SRC_AUDIO_BACKEND_AUDIOGRAPHBACKEND

#if _WIN32

#include "native/Native.hpp"

#include <QString>

#include <memory>

namespace YADAW::Audio::Backend
{
class AudioGraphBackend
{
    class Impl;
public:
    enum Status
    {
        Empty,
        Initialized,
        Created,
        Processing
    };
    struct DeviceInfo
    {
        QString name;
        QString id;
        bool isEnabled;
    };
    struct InterleaveAudioBuffer
    {
        std::uint8_t* data = nullptr;
        int channelCount = 0;
        int frameCount = 0;
        static constexpr int sampleSizeInBytes()
        {
            return sizeof(float);
        }
        inline std::uint8_t* at(std::uint32_t frameIndex, std::uint32_t channelIndex) const
        {
            return data + ((frameIndex * channelCount + channelIndex) * sampleSizeInBytes());
        }
    };
    using AudioCallbackType = void(int inputCount, const InterleaveAudioBuffer* inputs,
        int outputCount, const InterleaveAudioBuffer* outputs);
public:
    AudioGraphBackend();
    AudioGraphBackend(const AudioGraphBackend&) = delete;
    AudioGraphBackend(AudioGraphBackend&& rhs) noexcept;
    ~AudioGraphBackend();
public:
    bool initialize();
    bool uninitialize();
    std::uint32_t audioInputDeviceCount() const;
    std::uint32_t audioOutputDeviceCount() const;
    std::uint32_t defaultAudioInputDeviceIndex() const;
    std::uint32_t defaultAudioOutputDeviceIndex() const;
    QString defaultAudioInputDeviceId() const;
    QString defaultAudioOutputDeviceId() const;
    DeviceInfo audioInputDeviceAt(std::uint32_t index) const;
    DeviceInfo audioOutputDeviceAt(std::uint32_t index) const;
    YADAW::Native::ErrorCodeType createAudioGraph(std::uint32_t sampleRate = 0);
    YADAW::Native::ErrorCodeType createAudioGraph(const QString& id, std::uint32_t sampleRate = 0);
    bool isDeviceInputActivated(std::uint32_t deviceInputIndex) const;
    YADAW::Native::ErrorCodeType activateDeviceInput(std::uint32_t deviceInputIndex, bool enabled);
    DeviceInfo currentOutputDevice() const;
    void destroyAudioGraph();
    void start(AudioCallbackType* callback);
    void stop();
    int bufferSizeInFrames() const;
    int latencyInSamples() const;
    std::uint32_t sampleRate() const;
    std::uint32_t channelCount(bool isInput, std::uint32_t deviceIndex) const;
    Status status() const;
public:
    bool setOutputDeviceIndex(std::uint32_t index);
public:
    void swap(AudioGraphBackend& rhs);
private:
    std::unique_ptr<Impl> pImpl_;
    Status status_;
};

QString getAudioGraphErrorStringFromErrorCode(YADAW::Native::ErrorCodeType errorCode);
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_AUDIOGRAPHBACKEND
