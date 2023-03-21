#include "VST3AttributeList.hpp"

namespace YADAW::Audio::Host
{
VST3AttributeList::VST3AttributeList()
{
}

VST3AttributeList* VST3AttributeList::createAttributeList()
{
    // TODO: Use memory pool
    try
    {
        return new VST3AttributeList;
    }
    catch(...)
    {
        return nullptr;
    }
}

uint32 VST3AttributeList::addRef()
{
    return refCount_.fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
}

uint32 VST3AttributeList::release()
{
    auto ret = refCount_.fetch_sub(1, std::memory_order::memory_order_acq_rel) - 1;
    if(ret == 0)
    {
        delete this;
    }
    return ret;
}

tresult VST3AttributeList::queryInterface(const TUID _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, IAttributeList)
    QUERY_INTERFACE(_iid, obj, IAttributeList::iid, IAttributeList)
    *obj = nullptr;
    return kNoInterface;
}

tresult VST3AttributeList::setInt(IAttributeList::AttrID id, int64 value)
{
    try
    {
        map_[id].emplace<int64>(value);
        return kResultOk;
    }
    catch(...)
    {
        return kOutOfMemory;
    }
}

tresult VST3AttributeList::getInt(IAttributeList::AttrID id, int64& value)
{
    if(auto it = map_.find(id); it != map_.end())
    {
        auto ptrToValue = std::get_if<int64>(&(it->second));
        if(ptrToValue)
        {
            value = *ptrToValue;
            return kResultOk;
        }
    }
    return kNotInitialized;
}

tresult VST3AttributeList::setFloat(IAttributeList::AttrID id, double value)
{
    try
    {
        map_[id].emplace<double>(value);
        return kResultOk;
    }
    catch(...)
    {
        return kOutOfMemory;
    }
}

tresult VST3AttributeList::getFloat(IAttributeList::AttrID id, double& value)
{
    if(auto it = map_.find(id); it != map_.end())
    {
        auto ptrToValue = std::get_if<double>(&(it->second));
        if(ptrToValue)
        {
            value = *ptrToValue;
            return kResultOk;
        }
    }
    return kNotInitialized;
}

tresult VST3AttributeList::setString(IAttributeList::AttrID id, const TChar* string)
{
    if(string == nullptr)
    {
        return kInvalidArgument;
    }
    try
    {
        map_[id].emplace<std::u16string>(string);
        return kResultOk;
    }
    catch(...)
    {
        return kOutOfMemory;
    }
}

tresult VST3AttributeList::getString(IAttributeList::AttrID id, TChar* string, uint32 sizeInBytes)
{
    if(auto it = map_.find(id); it != map_.end())
    {
        auto ptrToValue = std::get_if<std::u16string>(&(it->second));
        if(ptrToValue)
        {
            if(sizeInBytes < ptrToValue->length() * sizeof(char16_t) + 2)
            {
                std::copy(ptrToValue->begin(), ptrToValue->end(), string);
                return kResultOk;
            }
            return kInvalidArgument;
        }
    }
    return kNotInitialized;
}

tresult VST3AttributeList::setBinary(IAttributeList::AttrID id, const void* data, uint32 sizeInBytes)
{
    if(data == nullptr)
    {
        return kInvalidArgument;
    }
    try
    {
        auto& vector = map_[id].emplace<std::vector<char>>();
        vector.resize(sizeInBytes);
        auto dataAsChar = reinterpret_cast<const char*>(data);
        std::copy(dataAsChar, dataAsChar + sizeInBytes, vector.begin());
        return kResultOk;
    }
    catch(...)
    {
        return kOutOfMemory;
    }
}

tresult VST3AttributeList::getBinary(IAttributeList::AttrID id, const void*& data, uint32& sizeInBytes)
{
    if(auto it = map_.find(id); it != map_.end())
    {
        if(const auto& ptrToValue = std::get_if<std::vector<char>>(&(it->second)); ptrToValue)
        {
            data = ptrToValue->data();
            sizeInBytes = ptrToValue->size();
            return kResultOk;
        }
    }
    return kNotInitialized;
}
}