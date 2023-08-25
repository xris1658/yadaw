#include "VST3DoubleBuffer.hpp"

using namespace YADAW::Audio::Host;

using namespace Steinberg::Vst;

std::pair<VST3EventList&, VST3EventList&> VST3DoubleBuffer::hostSideEventList()
{
    auto hostSideBufferIndex = hostSideBufferIndex_.load(std::memory_order::memory_order_acquire);
    return {
        inputs_[hostSideBufferIndex],
        outputs_[hostSideBufferIndex]
    };
}

std::pair<VST3EventList&, VST3EventList&> VST3DoubleBuffer::pluginSideEventList()
{
    auto pluginSideBufferIndex = hostSideBufferIndex_.load(std::memory_order::memory_order_acquire) ^ 1;
    return {
        inputs_[pluginSideBufferIndex],
        outputs_[pluginSideBufferIndex]
    };
}

void VST3DoubleBuffer::switchBuffer()
{
    hostSideBufferIndex_.fetch_xor(1);
}
