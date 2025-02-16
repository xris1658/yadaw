#include "AudioBufferPool.hpp"

#include "util/IntegerRange.hpp"

#include <cstring>

namespace YADAW::Audio::Util
{
constexpr std::size_t bufferCount = 64;

AudioBufferPool::AudioBufferPool(std::uint32_t singleBufferByteSize):
    singleBufferByteSize_(singleBufferByteSize),
    alignTypeCount_(alignTypeCount(singleBufferByteSize)),
    alignedPool_(),
    vacant_()
{
    alignedPool_.emplace_back(std::make_unique<AlignType[]>(alignTypeCount_ * bufferCount));
    std::memset(alignedPool_.back().get(), 0, alignTypeCount_ * sizeof(AlignType) * bufferCount);
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
    std::uint32_t row = alignedPool_.size();
    std::uint32_t column = 0U;
    {
        std::lock_guard<std::mutex> lg(mutex_);
        FOR_RANGE0(i, alignedPool_.size())
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
        if(row == alignedPool_.size())
        {
            alignedPool_.emplace_back(std::make_unique<AlignType[]>(alignTypeCount_ * bufferCount));
            std::memset(alignedPool_.back().get(), 0, alignTypeCount_ * sizeof(AlignType) * bufferCount);
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

std::uint32_t AudioBufferPool::alignTypeCount(
    std::uint32_t singleBufferByteSize)
{
    return (singleBufferByteSize + sizeof(AlignType) - 1) / sizeof(AlignType);
}

Buffer::Buffer(
    std::uint32_t row, std::uint32_t column,
    YADAW::Util::IntrusivePointer<AudioBufferPool> pool):
    pool_(std::move(pool)),
    pointer_(
        reinterpret_cast<std::byte*>(
            pool_->alignedPool_[row].get() + pool_->alignTypeCount_ * column
        )
    ),
    row_(row),
    column_(column)
{
    std::memset(pointer_, 0, pool_->singleBufferByteSize());
}

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