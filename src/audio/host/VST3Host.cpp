#include "VST3Host.hpp"

#include "audio/host/VST3AttributeList.hpp"
#include "audio/host/VST3Message.hpp"
#include "util/Base.hpp"

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;

VST3Host& VST3Host::instance()
{
    static VST3Host ret;
    return ret;
}

uint32 VST3Host::addRef()
{
    return 1;
}

uint32 VST3Host::release()
{
    return 1;
}

tresult VST3Host::queryInterface(const char* _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, IHostApplication)
    QUERY_INTERFACE(_iid, obj, IHostApplication::iid, IHostApplication)
    *obj = nullptr;
    return kNoInterface;
}

tresult VST3Host::getName(TChar* name)
{
    constexpr char16_t hostName[] = u"YADAW";
    std::memcpy(name, hostName, YADAW::Util::stackArraySize(hostName));
    return kResultOk;
}

tresult VST3Host::createInstance(char* cid, char* iid_, void** obj)
{
    auto classId = FUID::fromTUID(cid);
    auto interfaceId = FUID::fromTUID(iid_);
    if(classId == IAttributeList::iid && interfaceId == IAttributeList::iid)
    {
        *obj = VST3AttributeList::createAttributeList();
        if(*obj)
        {
            return kResultOk;
        }
        return kOutOfMemory;
    }
    if(classId == IMessage::iid && interfaceId == IMessage::iid)
    {
        *obj = VST3Message::createMessage();
        if(*obj)
        {
            return kResultOk;
        }
        return kOutOfMemory;
    }
    return kNoInterface;
}
}