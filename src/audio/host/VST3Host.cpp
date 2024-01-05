#include "VST3Host.hpp"

#include "audio/host/VST3AttributeList.hpp"
#include "audio/host/VST3Message.hpp"

#include <iterator>

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;

uint32 VST3Host::PlugInterfaceSupport::addRef()
{
    return 1;
}

uint32 VST3Host::PlugInterfaceSupport::release()
{
    return 1;
}

tresult VST3Host::PlugInterfaceSupport::queryInterface(const char* _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, FUnknown)
    QUERY_INTERFACE(_iid, obj, IPlugInterfaceSupport::iid, IPlugInterfaceSupport)
    *obj = nullptr;
    return kNoInterface;
}

tresult VST3Host::PlugInterfaceSupport::isPlugInterfaceSupported(const char* _iid)
{
    return Steinberg::kNotImplemented;
}

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
    if(FUnknownPrivate::iidEqual(_iid, IPlugInterfaceSupport::iid))
    {
        *obj = &plugInterfaceSupport_;
        return kResultOk;
    }
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, IHostApplication)
    QUERY_INTERFACE(_iid, obj, IHostApplication::iid, IHostApplication)
    *obj = nullptr;
    return kNoInterface;
}

tresult VST3Host::getName(TChar* name)
{
    constexpr char16_t hostName[] = u"YADAW";
    std::memcpy(name, hostName, std::size(hostName) * sizeof(char16_t));
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