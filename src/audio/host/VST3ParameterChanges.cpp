#include "VST3ParameterChanges.hpp"

namespace YADAW::Audio::Host
{
using namespace Steinberg;

VST3ParameterChanges::VST3ParameterChanges(std::size_t reserveParameterCount)
{
    reserve(reserveParameterCount);
}

VST3ParameterChanges::~VST3ParameterChanges() noexcept = default;

tresult VST3ParameterChanges::queryInterface(const int8* _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, Vst::IParameterChanges)
    QUERY_INTERFACE(_iid, obj, Vst::IParameterChanges::iid, Vst::IParameterChanges)
    *obj = nullptr;
    return kNoInterface;
}

uint32 VST3ParameterChanges::addRef()
{
    return 1;
}

uint32 VST3ParameterChanges::release()
{
    return 1;
}

int32 VST3ParameterChanges::getParameterCount()
{
    return parameterValueQueues_.size();
}

Vst::IParamValueQueue* VST3ParameterChanges::getParameterData(int32 index)
{
    if(index >= 0 && index < parameterValueQueues_.size())
    {
        return &(parameterValueQueues_[index]);
    }
    return nullptr;
}

Vst::IParamValueQueue* VST3ParameterChanges::addParameterData(const Vst::ParamID& id, int32& index)
{
    auto it = std::find_if(parameterValueQueues_.begin(), parameterValueQueues_.end(),
        [&id](const VST3ParameterValueQueue& queue) { return queue.paramId() == id; });
    if(it != parameterValueQueues_.end())
    {
        index = it - parameterValueQueues_.begin();
        return &*it;
    }
    auto ret = parameterValueQueues_.size();
    try
    {
        parameterValueQueues_.emplace_back(id);
        index = ret;
        return &(parameterValueQueues_.back());
    }
    catch(...)
    {
        index = -1;
        return nullptr;
    }
}

void VST3ParameterChanges::reserve(std::size_t reserveParameterCount)
{
    parameterValueQueues_.reserve(reserveParameterCount);
}

void VST3ParameterChanges::reset()
{
    parameterValueQueues_.clear();
}

void VST3ParameterChanges::clearPointsInQueue()
{
    for(auto& queue: parameterValueQueues_)
    {
        queue.clear();
    }
}
}