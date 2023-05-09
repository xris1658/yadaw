#ifndef YADAW_SRC_NATIVE_AUDIOGRAPHBACKEND
#define YADAW_SRC_NATIVE_AUDIOGRAPHBACKEND

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
    enum DeviceInputResult
    {
        Failed,
        Success,
        AlreadyDone
    };
    enum class SampleFormat
    {
        Int16,
        Int24,
        Float32,
        Float64
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
        SampleFormat sampleFormat = SampleFormat::Int16;
        static int sampleSizeInBytes(SampleFormat sampleFormat)
        {
            return sampleFormat == SampleFormat::Int16? 2:
            sampleFormat == SampleFormat::Int24? 3:
            sampleFormat == SampleFormat::Float32? 4:
            sampleFormat == SampleFormat::Float64? 8:
            0;
        }
        inline std::uint8_t* at(std::uint32_t frameIndex, std::uint32_t channelIndex) const
        {
            return data + ((frameIndex * channelCount + channelIndex) * sampleSizeInBytes(sampleFormat));
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
    DeviceInfo audioInputDeviceAt(std::uint32_t index) const;
    DeviceInfo audioOutputDeviceAt(std::uint32_t index) const;
    bool createAudioGraph();
    bool createAudioGraph(const QString& id);
    bool isDeviceInputActivated(std::uint32_t deviceInputIndex) const;
    DeviceInputResult activateDeviceInput(std::uint32_t deviceInputIndex, bool enabled);
    // This function might fail, in which case returns a blank DeviceInfo
    DeviceInfo currentOutputDevice() const;
    void destroyAudioGraph();
    void start(AudioCallbackType* callback);
    void stop();
    int bufferSizeInFrames() const;
    int latencyInSamples() const;
    std::uint32_t sampleRate() const;
    std::uint32_t channelCount(bool isInput, std::uint32_t deviceIndex) const;
public:
    bool setOutputDeviceIndex(std::uint32_t index);
public:
    void swap(AudioGraphBackend& rhs);
private:
    std::unique_ptr<Impl> pImpl_;
    Status status_;
};
}

#endif //YADAW_SRC_NATIVE_AUDIOGRAPHBACKEND
