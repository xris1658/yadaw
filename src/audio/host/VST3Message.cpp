#include "VST3Message.hpp"

namespace YADAW::Audio::Host
{
using namespace Steinberg;
using namespace Steinberg::Vst;

VST3Message::VST3Message()
{
}

VST3Message* VST3Message::createMessage()
{
    // TODO: Use memory pool
    try
    {
        return new VST3Message;
    }
    catch(...)
    {
        return nullptr;
    }
}

VST3Message::~VST3Message() noexcept
{
    attributeList_->release();
    attributeList_ = nullptr;
}

uint32 VST3Message::addRef()
{
    return ++refCount_;
}

uint32 VST3Message::release()
{
    --refCount_;
    auto ret = refCount_;
    if(refCount_ == 0)
    {
        delete this;
    }
    return ret;
}

tresult VST3Message::queryInterface(const char* _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, IMessage)
    QUERY_INTERFACE(_iid, obj, IMessage::iid, IMessage)
    *obj = nullptr;
    return kNoInterface;
}

FIDString VST3Message::getMessageID()
{
    return id_.c_str();
}

void VST3Message::setMessageID(FIDString id)
{
    id_ = id;
}

// FIXME: Not thread-safe
IAttributeList* VST3Message::getAttributes()
{
    if(!attributeList_)
    {
        attributeList_ = VST3AttributeList::createAttributeList();
    }
    return attributeList_;
}
}