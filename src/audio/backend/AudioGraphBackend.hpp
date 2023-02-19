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
        Created,
        Processing
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
        InterleaveAudioBuffer() = default;
        InterleaveAudioBuffer(const InterleaveAudioBuffer&) = default;
        InterleaveAudioBuffer(InterleaveAudioBuffer&&) noexcept = default;
        InterleaveAudioBuffer& operator=(const InterleaveAudioBuffer&) = default;
        InterleaveAudioBuffer& operator=(InterleaveAudioBuffer&&) noexcept = default;
        ~InterleaveAudioBuffer() noexcept = default;
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
        inline std::uint8_t* at(int frameIndex, int channelIndex) const
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
    int audioInputDeviceCount() const;
    int audioOutputDeviceCount() const;
    DeviceInfo audioInputDeviceAt(int index) const;
    DeviceInfo audioOutputDeviceAt(int index) const;
    bool createAudioGraph();
    bool createAudioGraph(const QString& id);
    int enableDeviceInput(const QString& id);
    bool disableDeviceInput(int deviceInputIndex);
    // This function might fail, in which case returns a blank DeviceInfo
    DeviceInfo currentOutputDevice() const;
    int currentInputDeviceCount() const;
    DeviceInfo currentInputDeviceAt() const;
    void destroyAudioGraph();
    void start(AudioCallbackType* callback);
    void stop();
    int bufferSizeInFrames() const;
    int latencyInSamples() const;
    std::uint32_t sampleRate() const;
public:
    void swap(AudioGraphBackend& rhs);
private:
    std::unique_ptr<Impl> pImpl_;
    Status status_;
};
}

#endif //YADAW_SRC_NATIVE_AUDIOGRAPHBACKEND
