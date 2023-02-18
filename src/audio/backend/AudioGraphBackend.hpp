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
        static int sampleSizeInBytes(SampleFormat sampleFormat)
        {
            return sampleFormat == SampleFormat::Int16? 2:
            sampleFormat == SampleFormat::Int24? 3:
            sampleFormat == SampleFormat::Float32? 4:
            sampleFormat == SampleFormat::Float64? 8:
            0;
        }
        std::uint8_t* const data;
        int channelCount;
        int frameCount;
        SampleFormat sampleFormat;
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
    int audioOutputDeviceCount() const;
    DeviceInfo audioOutputDeviceAt(int index) const;
    bool createAudioGraph();
    bool createAudioGraph(const QString& id);
    // This function might fail, in which case returns a blank DeviceInfo
    DeviceInfo currentOutputDevice() const;
    void destroyAudioGraph();
    void start(AudioCallbackType* callback);
    void stop();
public:
    std::uint32_t sampleRate() const;
public:
    void swap(AudioGraphBackend& rhs);
private:
    std::unique_ptr<Impl> pImpl_;
    Status status_;
};
}

#endif //YADAW_SRC_NATIVE_AUDIOGRAPHBACKEND
