#ifndef YADAW_BASE_FIXEDSIZEMEMORYBLOCK
#define YADAW_BASE_FIXEDSIZEMEMORYBLOCK

#include "util/Base.hpp"

#include <cstddef>

namespace YADAW::Util
{
class FixedSizeMemoryBlock
{
public:
    explicit FixedSizeMemoryBlock(std::size_t blockSize = 0):
        blockSize_(blockSize),
        block_(nullptr)
    {
        if(blockSize_)
        {
            block_ = new std::byte[blockSize];
        }
    }
    FixedSizeMemoryBlock(const FixedSizeMemoryBlock& rhs) = delete;
    FixedSizeMemoryBlock(FixedSizeMemoryBlock&& rhs) noexcept:
        blockSize_(rhs.blockSize_),
        block_(rhs.block_)
    {
        rhs.blockSize_ = 0U;
        // rhs.block_ = nullptr;
    }
    FixedSizeMemoryBlock& operator=(FixedSizeMemoryBlock&& rhs) noexcept
    {
        std::swap(blockSize_, rhs.blockSize_);
        std::swap(block_, rhs.block_);
        return *this;
    }
    ~FixedSizeMemoryBlock()
    {
        if(blockSize_)
        {
            delete[] block_;
        }
    }
public:
    void init()
    {
        std::memset(block_, 0, blockSize());
    }
    std::byte* data() const noexcept
    {
        return block_;
    }
    std::size_t blockSize() const noexcept
    {
        return blockSize_;
    }
private:
    std::size_t blockSize_;
    std::byte* block_ = nullptr;
};
}

#endif //YADAW_BASE_FIXEDSIZEMEMORYBLOCK
