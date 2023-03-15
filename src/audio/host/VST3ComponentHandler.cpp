#include "VST3ComponentHandler.hpp"

#include "audio/plugin/VST3Plugin.hpp"
#include "native/Native.hpp"

namespace YADAW::Audio::Host
{
constexpr auto nanosecondCount = 1000000000;

VST3ComponentHandler::VST3ComponentHandler(YADAW::Audio::Plugin::VST3Plugin* plugin):
    plugin_(plugin),
    bufferIndex_(0),
    timestamp_(0),
    parameterChanges_{VST3ParameterChanges(plugin_->parameter()->parameterCount()),
        VST3ParameterChanges(plugin_->parameter()->parameterCount())},
    mappings_{{}, {}}
{
}

VST3ComponentHandler::~VST3ComponentHandler() noexcept
{
}

uint32 VST3ComponentHandler::addRef()
{
    return 1;
}

uint32 VST3ComponentHandler::release()
{
    return 1;
}

tresult VST3ComponentHandler::queryInterface(const char* _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, IComponentHandler)
    QUERY_INTERFACE(_iid, obj, IComponentHandler::iid, IComponentHandler)
    *obj = nullptr;
    return Steinberg::kNoInterface;
}

int32 VST3ComponentHandler::doBeginEdit(int bufferIndex, ParamID id)
{
    int index = -1;
    parameterChanges_[bufferIndex].addParameterData(id, index);
    return index;
}

tresult VST3ComponentHandler::doPerformEdit(int bufferIndex, int32 index,
    ParamValue normalizedValue, std::int64_t timestamp)
{
    int32 sampleOffset = (timestamp - timestamp_) / (sampleRate() * nanosecondCount);
    int32 pointIndex = -1;
    return parameterChanges_[bufferIndex].getParameterData(index)->addPoint(sampleOffset, normalizedValue, pointIndex);
}

tresult VST3ComponentHandler::beginEdit(ParamID id)
{
    auto bufferIndex = bufferIndex_;
    auto iterator = std::find_if(mappings_[bufferIndex].begin(), mappings_[bufferIndex].end(),
        [id](const auto& mapping)
        {
            const auto& [mappingId, index] = mapping;
            return mappingId == id;
        }
    );
    if(iterator == mappings_[bufferIndex].end())
    {
        if(auto index = doBeginEdit(bufferIndex, id); index != -1)
        {
            mappings_[bufferIndex].emplace_back(id, index);
            return kResultOk;
        }
        return kResultFalse;
    }
    return kResultOk;
}

tresult VST3ComponentHandler::performEdit(ParamID id, ParamValue normalizedValue)
{
    auto bufferIndex = bufferIndex_;
    auto timestamp = YADAW::Native::currentTimeValueInNanosecond();
    auto iterator = std::find_if(mappings_[bufferIndex].begin(), mappings_[bufferIndex].end(),
        [id](const auto& mapping)
        {
            const auto& [mappingId, index] = mapping;
            return mappingId == id;
        }
    );
    if(iterator == mappings_[bufferIndex].end())
    {
        if(auto index = doBeginEdit(bufferIndex, id); index != -1)
        {
            if(auto performEditResult = doPerformEdit(bufferIndex, index, normalizedValue, timestamp);
                performEditResult == kResultOk)
            {
                return endEdit(id);
            }
        }
        return kResultFalse;
    }
    const auto& [mappingId, mappingIndex] = *iterator;
    return doPerformEdit(bufferIndex, mappingIndex, normalizedValue, timestamp);
}

tresult VST3ComponentHandler::endEdit(ParamID id)
{
    // TODO: Not sure what to do yet :(
    return kResultOk;
}

tresult VST3ComponentHandler::restartComponent(int32 flags)
{
    using YADAW::Audio::Plugin::IPlugin;
    if(flags & RestartFlags::kReloadComponent)
    {
        auto status = plugin_->status();
        plugin_->stopProcessing();
        plugin_->deactivate();
        const auto& processSetup = plugin_->processSetup();
        auto sampleRate = processSetup.sampleRate;
        auto maxSamplesPerBlock = processSetup.maxSamplesPerBlock;
        plugin_->uninitialize();
        plugin_->initialize(sampleRate, maxSamplesPerBlock);
        if(status >= IPlugin::Status::Activated)
        {
            plugin_->activate();
        }
        if(status >= IPlugin::Status::Processing)
        {
            plugin_->startProcessing();
        }
        return kResultOk;
    }
    if((flags & RestartFlags::kIoChanged)
    || (flags & RestartFlags::kLatencyChanged))
    {
        auto status = plugin_->status();
        plugin_->startProcessing();
        plugin_->deactivate();
        // TODO: Query bus configuration and/or latency according to the flags
        plugin_->activate();
        if(status >= IPlugin::Status::Processing)
        {
            plugin_->startProcessing();
        }
        return kResultOk;
    }
    if(flags & RestartFlags::kParamTitlesChanged)
    {
        // TODO: Flush param info
        return kNotImplemented;
    }
    if(flags & RestartFlags::kParamValuesChanged)
    {
        // TODO: Flush param value
        return kNotImplemented;
    }
    return kNotImplemented;
}

void VST3ComponentHandler::switchBuffer(std::int64_t switchTimestampInNanosecond)
{
    timestamp_ = switchTimestampInNanosecond;
    if(bufferIndex_ == 0)
    {
        bufferIndex_ = 1;
    }
    else
    {
        bufferIndex_ = 0;
    }
    auto bufferIndex = bufferIndex_;
    parameterChanges_[bufferIndex].clear();
    mappings_[bufferIndex].clear();
}

void VST3ComponentHandler::consumeParameterChanges(Vst::ProcessData& processData)
{
    auto bufferIndex = bufferIndex_ == 0? 1: 0;
    processData.inputParameterChanges = &(parameterChanges_[bufferIndex]);
}

double VST3ComponentHandler::sampleRate() const
{
    return plugin_->processSetup().sampleRate;
}
}