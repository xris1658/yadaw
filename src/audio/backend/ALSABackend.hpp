#ifndef YADAW_SRC_AUDIO_BACKEND_ALSAAUDIOBACKEND
#define YADAW_SRC_AUDIO_BACKEND_ALSAAUDIOBACKEND

#if __linux__

#include "audio/backend/ALSADeviceSelector.hpp"

#include <cstdint>
#include <memory>
#include <optional>

namespace YADAW::Audio::Backend
{
class ALSABackend
{
    class Impl;
public:
    enum ActivateDeviceResult
    {
        Failed,
        Success,
        AlreadyDone
    };
    enum SampleFormat: std::uint8_t
    {
        Unknown,
        DoubleFloat,
        DoubleFloatRE,
        Float,
        FloatRE,
        Int32,
        Int32RE,
        Int24,
        Int24RE,
        Int20,
        Int20RE,
        Int16,
        Int16RE,
        Int8
    };
    static constexpr std::size_t sampleSize[] = {
        0, 8, 8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 1
    };
    struct AudioBuffer
    {
        SampleFormat format = SampleFormat::Unknown;
        bool interleaved = false;
        std::uint32_t channelCount = 0;
        union
        {
            void* interleavedBuffer;
            void** nonInterleavedBuffer;
        } buffer;
    };
    using Callback = void(const ALSABackend* backend,
        std::uint32_t inputCount, AudioBuffer* const inputBuffers,
        std::uint32_t outputCount, AudioBuffer* const outputBuffers);
public:
    ALSABackend();
    ALSABackend(const ALSABackend&) = delete;
    ALSABackend(ALSABackend&& rhs) noexcept;
    ~ALSABackend();
public:
    bool initialize(std::uint32_t sampleRate, std::uint32_t frameCount);
    bool uninitialize();
    std::uint32_t sampleRate() const;
    std::uint32_t frameCount() const;
    static std::uint32_t audioInputDeviceCount();
    static std::uint32_t audioOutputDeviceCount();
    static std::optional<ALSADeviceSelector> audioInputDeviceAt(std::uint32_t index);
    static std::optional<ALSADeviceSelector> audioOutputDeviceAt(std::uint32_t index);
    static std::optional<std::string> audioDeviceName(ALSADeviceSelector selector);
    static std::optional<std::string> cardName(int cardIndex);
    ActivateDeviceResult setAudioDeviceActivated(bool isInput, std::uint32_t index, bool activated);
    bool isAudioDeviceActivated(bool isInput, std::uint32_t index) const;
    std::uint32_t channelCount(bool isInput, std::uint32_t index) const;
    bool start(const Callback* callback);
    bool stop();
private:
    std::unique_ptr<Impl> pImpl_;
};

std::optional<std::uint32_t> findDeviceBySelector(
    const ALSABackend& backend, bool isInput, ALSADeviceSelector selector);
}

#endif

#endif // YADAW_SRC_AUDIO_BACKEND_ALSAAUDIOBACKEND
