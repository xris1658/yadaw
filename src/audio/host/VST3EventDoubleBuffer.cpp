#include "VST3EventDoubleBuffer.hpp"

#include "audio/host/HostContext.hpp"

#include <mutex>

namespace YADAW::Audio::Host
{
using namespace YADAW::Util;

OptionalRef<Steinberg::Vst::Event> VST3EventDoubleBuffer::emplaceInputEvent()
{
    return inputs_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get()].emplace();
}

YADAW::Audio::Host::VST3EventList& VST3EventDoubleBuffer::hostOutputEventList()
{
    return outputs_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get()];
}

std::pair<YADAW::Audio::Host::VST3EventList&, YADAW::Audio::Host::VST3EventList&>
    VST3EventDoubleBuffer::pluginSideEventList()
{
    auto hostSideBufferIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get() ^ 1;
    return {inputs_[hostSideBufferIndex], outputs_[hostSideBufferIndex]};
}

void VST3EventDoubleBuffer::bufferSwitched()
{
    auto index = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    outputs_[index ^ 1].clear();
    inputs_[index].clear();
}

}