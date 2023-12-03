#include "AudioBufferPool.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Util
{
constexpr std::size_t bufferCount = 64;

AudioBufferPool::AudioBufferPool(std::uint32_t singleBufferByteSize):
    singleBufferByteSize_(singleBufferByteSize),
    pool_(),
    vacant_()
{
    pool_.emplace_back(std::make_unique<std::vector<std::byte>>(bufferCount * singleBufferByteSize_));
    vacant_.emplace_back(bufferCount, true);
}

YADAW::Util::IntrusivePointer<AudioBufferPool> AudioBufferPool::createPool(
    std::uint32_t singleBufferSampleCount)
{
    return YADAW::Util::IntrusivePointer<AudioBufferPool>(
        new(std::nothrow) AudioBufferPool(singleBufferSampleCount));
}

std::size_t AudioBufferPool::singleBufferByteSize() const
{
    return singleBufferByteSize_;
}

Buffer AudioBufferPool::lend()
{
    std::uint32_t row = pool_.size();
    std::uint32_t column = 0U;
    {
        std::lock_guard<std::mutex> lg(mutex_);
        FOR_RANGE0(i, pool_.size())
        {
            FOR_RANGE0(j, bufferCount)
            {
                if(vacant_[i][j])
                {
                    row = i;
                    column = j;
                    break;
                }
            }
        }
        if(row == pool_.size())
        {
            pool_.emplace_back(
                std::make_unique<std::vector<std::byte>>(
                    bufferCount * singleBufferByteSize_
                )
            );
            vacant_.emplace_back(bufferCount, true);
        }
        vacant_[row][column] = false;
    }
    return {row, column, intrusiveFromThis()};
}

YADAW::Util::IntrusivePointer<AudioBufferPool> AudioBufferPool::intrusiveFromThis()
{
    addRef();
    return YADAW::Util::IntrusivePointer<AudioBufferPool>(this);
}

Buffer::Buffer(
    std::uint32_t row, std::uint32_t column,
    YADAW::Util::IntrusivePointer<AudioBufferPool> pool):
    pool_(pool),
    pointer_(pool_->pool_[row]->data() + pool_->singleBufferByteSize_ * column),
    row_(row),
    column_(column)
{}

Buffer::Buffer(Buffer&& rhs) noexcept:
    pool_(std::move(rhs.pool_)),
    pointer_(rhs.pointer_),
    row_(rhs.row_),
    column_(rhs.column_)
{}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept
{
    if(this != &rhs)
    {
        pool_ = std::move(rhs.pool_);
        pointer_ = rhs.pointer_;
        row_ = rhs.row_;
        column_ = rhs.column_;
    }
    return *this;
}

Buffer::~Buffer()
{
    if(auto pool = pool_.get(); pool)
    {
        std::lock_guard<std::mutex> lg(pool->mutex_);
        pool->vacant_[row_][column_] = true;
        // std::memset(ptr, 0, pool->singleBufferByteSize_);
    }
}
}