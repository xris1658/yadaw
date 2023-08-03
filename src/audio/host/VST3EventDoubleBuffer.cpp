#include "VST3EventDoubleBuffer.hpp"

namespace YADAW::Audio::Host
{
using namespace YADAW::Util;

OptionalRef<Steinberg::Vst::Event> VST3EventDoubleBuffer::emplaceInputEvent()
{
    AtomicLockGuard lg(mutex_);
    return inputs_[hostSideBufferIndex_].emplace();
}

YADAW::Audio::Host::VST3EventList& VST3EventDoubleBuffer::hostOutputEventList()
{
    AtomicLockGuard lg(mutex_);
    return outputs_[hostSideBufferIndex_];
}

std::pair<YADAW::Audio::Host::VST3EventList&, YADAW::Audio::Host::VST3EventList&>
    VST3EventDoubleBuffer::hostSideEventList()
{
    int hostSideBufferIndex;
    {
        AtomicLockGuard lg(mutex_);
        hostSideBufferIndex = hostSideBufferIndex_;
    }
    return {inputs_[hostSideBufferIndex], outputs_[hostSideBufferIndex]};
}

std::pair<YADAW::Audio::Host::VST3EventList&, YADAW::Audio::Host::VST3EventList&>
    VST3EventDoubleBuffer::pluginSideEventList()
{
    int hostSideBufferIndex;
    {
        AtomicLockGuard lg(mutex_);
        hostSideBufferIndex = hostSideBufferIndex_ ^ 1;
    }
    return {inputs_[hostSideBufferIndex], outputs_[hostSideBufferIndex]};
}

void VST3EventDoubleBuffer::switchBuffer()
{
    AtomicLockGuard lg(mutex_);
    outputs_[hostSideBufferIndex_].clear();
    hostSideBufferIndex_ ^= 1;
    inputs_[hostSideBufferIndex_].clear();
}

}