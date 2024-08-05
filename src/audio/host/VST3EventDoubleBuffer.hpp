#ifndef YADAW_SRC_AUDIO_HOST_VST3EVENTDOUBLEBUFFER
#define YADAW_SRC_AUDIO_HOST_VST3EVENTDOUBLEBUFFER

#include "audio/host/VST3EventList.hpp"
#include "util/AtomicMutex.hpp"
#include "util/OptionalUtil.hpp"

namespace YADAW::Audio::Host
{
class VST3EventDoubleBuffer
{
public:
    OptionalRef<Steinberg::Vst::Event> emplaceInputEvent();
    YADAW::Audio::Host::VST3EventList& hostOutputEventList();
    std::pair<YADAW::Audio::Host::VST3EventList&, YADAW::Audio::Host::VST3EventList&> pluginSideEventList();
    void bufferSwitched();
private:
    YADAW::Audio::Host::VST3EventList inputs_[2];
    YADAW::Audio::Host::VST3EventList outputs_[2];
};
}
#endif // YADAW_SRC_AUDIO_HOST_VST3EVENTDOUBLEBUFFER
