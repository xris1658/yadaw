#ifndef YADAW_SRC_AUDIO_UTIL_AUDIOBUFFERPOOL
#define YADAW_SRC_AUDIO_UTIL_AUDIOBUFFERPOOL

#include "util/IntrusivePointer.hpp"

#include <memory>
#include <mutex>
#include <vector>

namespace YADAW::Audio::Util
{
class Buffer;

class AudioBufferPool:
    public YADAW::Util::IntrusiveRefCounter
{
    friend class Buffer;
public:
    using Buffer = YADAW::Audio::Util::Buffer;
private:
    AudioBufferPool(std::uint32_t singleBufferByteSize);
    static YADAW::Util::IntrusivePointer<AudioBufferPool> createPool(std::uint32_t singleBufferSampleCount);
public:
    template<typename SampleType>
    static YADAW::Util::IntrusivePointer<AudioBufferPool> createPool(std::uint32_t singleBufferSampleCount)
    {
        return createPool(singleBufferSampleCount * sizeof(SampleType));
    }
public:
    AudioBufferPool(const AudioBufferPool&) = delete;
    ~AudioBufferPool() = default;
public:
    std::size_t singleBufferByteSize() const;
    Buffer lend();
private:
    YADAW::Util::IntrusivePointer<AudioBufferPool> intrusiveFromThis();
private:
    std::uint32_t singleBufferByteSize_;
    std::vector<std::unique_ptr<std::vector<std::byte>>> pool_;
    std::vector<std::vector<bool>> vacant_;
    std::mutex mutex_;
};

class Buffer
{
    friend AudioBufferPool;
public:
    Buffer() {}
private:
    Buffer(std::uint32_t row, std::uint32_t column,
        YADAW::Util::IntrusivePointer<AudioBufferPool> pool);
public:
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&& rhs) noexcept;
    Buffer& operator=(Buffer&& rhs) noexcept;
    ~Buffer();
public:
    std::byte* pointer() const { return pointer_; }
private:
    YADAW::Util::IntrusivePointer<AudioBufferPool> pool_;
    std::byte* pointer_ = nullptr;
    std::uint32_t row_;
    std::uint32_t column_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_AUDIOBUFFERPOOL
