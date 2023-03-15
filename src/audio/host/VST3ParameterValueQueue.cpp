#include "VST3ParameterValueQueue.hpp"

namespace YADAW::Audio::Host
{
using namespace Steinberg;

VST3ParameterValueQueue::VST3ParameterValueQueue(Vst::ParamID paramId):
    paramId_(paramId)
{
}

tresult VST3ParameterValueQueue::queryInterface(const char* _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, Vst::IParamValueQueue)
    QUERY_INTERFACE(_iid, obj, Vst::IParamValueQueue::iid, Vst::IParamValueQueue)
    *obj = nullptr;
    return kNoInterface;
}

uint32 VST3ParameterValueQueue::addRef()
{
    return 1;
}

uint32 VST3ParameterValueQueue::release()
{
    return 1;
}

Vst::ParamID VST3ParameterValueQueue::getParameterId()
{
    return paramId_;
}

int32 VST3ParameterValueQueue::getPointCount()
{
    return container_.size();
}

tresult VST3ParameterValueQueue::getPoint(int32 index, int32& sampleOffset, Vst::ParamValue& value)
{
    if(index >= 0 && index < container_.size())
    {
        const auto& point = container_[index];
        sampleOffset = point.sampleOffset_;
        value = point.paramValue_;
        return kResultOk;
    }
    return kInvalidArgument;
}

tresult VST3ParameterValueQueue::addPoint(int32 sampleOffset, Vst::ParamValue value, int32& index)
{
    if(!container_.full())
    {
        index = container_.size();
        container_.emplaceBack(sampleOffset, value);
        return kResultOk;
    }
    return kOutOfMemory;
}

void VST3ParameterValueQueue::clear()
{
    container_.clear();
}
}