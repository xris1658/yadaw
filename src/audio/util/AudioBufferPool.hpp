#ifndef YADAW_SRC_AUDIO_UTIL_AUDIOBUFFERPOOL
#define YADAW_SRC_AUDIO_UTIL_AUDIOBUFFERPOOL

#include <memory>
#include <mutex>
#include <vector>

namespace YADAW::Audio::Util
{
class AudioBufferPool: public std::enable_shared_from_this<AudioBufferPool>
{
public:
    class Buffer
    {
        friend AudioBufferPool;
    public:
        Buffer() {}
    private:
        Buffer(std::size_t row, std::size_t column, std::shared_ptr<AudioBufferPool> pool);
    public:
        Buffer(const Buffer&) = delete;
        Buffer(Buffer&& rhs) noexcept;
        Buffer& operator=(Buffer&& rhs) noexcept;
        ~Buffer();
    public:
        std::byte* pointer() const { return pointer_; }
    private:
        std::shared_ptr<AudioBufferPool> pool_ = nullptr;
        std::byte* pointer_ = nullptr;
        std::size_t row_;
        std::size_t column_;
    };
private:
    AudioBufferPool(std::uint32_t singleBufferByteSize);
    static std::shared_ptr<AudioBufferPool> createPool(std::uint32_t singleBufferByteSize);
public:
    template<typename SampleType>
    static std::shared_ptr<AudioBufferPool> createPool(std::uint32_t singleBufferSampleCount)
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
    std::uint32_t singleBufferByteSize_;
    std::vector<std::unique_ptr<std::vector<std::byte>>> pool_;
    std::vector<std::vector<bool>> vacant_;
    std::mutex mutex_;
};
}

#endif // YADAW_SRC_AUDIO_UTIL_AUDIOBUFFERPOOL
