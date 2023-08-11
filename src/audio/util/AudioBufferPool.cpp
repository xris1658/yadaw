#include "AudioBufferPool.hpp"

namespace YADAW::Audio::Util
{
constexpr std::size_t bufferCount = 64;

AudioBufferPool::Buffer::Buffer(
    std::size_t row, std::size_t column, std::shared_ptr<AudioBufferPool> pool):
    pool_(std::move(pool)),
    pointer_(pool_->pool_[row]->data() + pool_->singleBufferByteSize_ * column),
    row_(row),
    column_(column)
{}

AudioBufferPool::Buffer::Buffer(AudioBufferPool::Buffer&& rhs) noexcept:
    pointer_(rhs.pointer_),
    row_(rhs.row_),
    column_(rhs.column_),
    pool_(std::move(rhs.pool_))
{}

AudioBufferPool::Buffer& AudioBufferPool::Buffer::operator=(AudioBufferPool::Buffer&& rhs) noexcept
{
    if(this != &rhs)
    {
        pointer_ = rhs.pointer_;
        row_ = rhs.row_;
        column_ = rhs.column_;
        pool_ = std::move(rhs.pool_);
    }
    return *this;
}

AudioBufferPool::Buffer::~Buffer()
{
    if(pool_)
    {
        std::lock_guard<std::mutex> lg(pool_->mutex_);
        pool_->vacant_[row_][column_] = true;
        // std::memset(ptr, 0, pool->singleBufferByteSize_);
    }
}

AudioBufferPool::AudioBufferPool(std::uint32_t singleBufferByteSize):
    singleBufferByteSize_(singleBufferByteSize),
    pool_(),
    vacant_()
{
    pool_.emplace_back(std::make_unique<std::vector<std::byte>>(bufferCount * singleBufferByteSize_));
    vacant_.emplace_back(bufferCount, true);
}

std::shared_ptr<AudioBufferPool> AudioBufferPool::createPool(std::uint32_t singleBufferByteSize)
{
    struct MakeSharedEnabler: public AudioBufferPool
    {
        MakeSharedEnabler(std::uint32_t singleBufferByteSize): AudioBufferPool(singleBufferByteSize) {}
    };
    return std::make_shared<MakeSharedEnabler>(singleBufferByteSize);
}

std::size_t AudioBufferPool::singleBufferByteSize() const
{
    return singleBufferByteSize_;
}

AudioBufferPool::Buffer AudioBufferPool::lend()
{
    std::size_t row = -1;
    std::size_t column = 0;
    {
        std::lock_guard<std::mutex> lg(mutex_);
        for(std::size_t i = 0; row == -1 && i < pool_.size(); ++i)
        {
            for(std::size_t j = 0; j < bufferCount; ++j)
            {
                if(vacant_[i][j])
                {
                    row = i;
                    column = j;
                    break;
                }
            }
        }
        if(row == -1)
        {
            pool_.emplace_back(
                std::make_unique<std::vector<std::byte>>(
                    bufferCount * singleBufferByteSize_
                )
            );
            vacant_.emplace_back(bufferCount, true);
            row = pool_.size() - 1;
            column = 0;
        }
        vacant_[row][column] = false;
    }
    return {row, column, shared_from_this()};
}
}