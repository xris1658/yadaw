#include "audio/util/AudioBufferPool.hpp"

#include <vector>

int main()
{
    auto bufferPool = YADAW::Audio::Util::AudioBufferPool::createPool(256 * sizeof(float));
    std::vector<YADAW::Audio::Util::AudioBufferPool::Buffer> vec;
    vec.reserve(128);
    for(int i = 0; i < 128; ++i)
    {
        vec.emplace_back(bufferPool->lend());
    }
    for(int i = 0; i < 128; ++i)
    {
        vec[i] = {};
    }
    return 0;
}