#include "VST3EventList.hpp"

namespace YADAW::Audio::Host
{
using namespace Steinberg;

VST3EventList::VST3EventList() {}

tresult VST3EventList::queryInterface(const char* _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, Vst::IEventList)
    QUERY_INTERFACE(_iid, obj, Vst::IEventList::iid, Vst::IEventList)
    *obj = nullptr;
    return kNoInterface;
}

uint32 VST3EventList::addRef()
{
    return 1;
}

uint32 VST3EventList::release()
{
    return 1;
}

int32 VST3EventList::getEventCount()
{
    return container_.size();
}

tresult VST3EventList::getEvent(int32 index, Vst::Event& e)
{
    if(index < getEventCount())
    {
        e = container_[index];
        return kResultOk;
    }
    return kInvalidArgument;
}

tresult VST3EventList::addEvent(Vst::Event& e)
{
    if(container_.pushBack(e))
    {
        return kResultOk;
    }
    return kOutOfMemory;
}

OptionalRef<Vst::Event> VST3EventList::emplace()
{
    if(container_.emplaceBack())
    {
        return {container_.back()};
    }
    return std::nullopt;
}

void VST3EventList::clear()
{
    container_.clear();
}
}