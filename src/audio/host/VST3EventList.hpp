#ifndef YADAW_SRC_AUDIO_HOST_VST3EVENTLIST
#define YADAW_SRC_AUDIO_HOST_VST3EVENTLIST

#include "util/FixedSizeCircularDeque.hpp"

#include <pluginterfaces/vst/ivstevents.h>

namespace YADAW::Audio::Host
{
using namespace Steinberg;
class VST3EventList: public Vst::IEventList
{
public:
    VST3EventList();
    ~VST3EventList() = default;
public: // FUnknown interfaces
    tresult queryInterface(const TUID _iid, void** obj) override;
    uint32 addRef() override;
    uint32 release() override;
public:
    int32 getEventCount() override;
    tresult getEvent(int32 index, Vst::Event& e) override;
    tresult addEvent(Vst::Event& e) override;
public:
    void clear();
private:
    YADAW::Util::FixedSizeCircularDeque<Vst::Event, 2048> container_;
};
}

#endif //YADAW_SRC_AUDIO_HOST_VST3EVENTLIST
