#include "VST3ComponentHandler.hpp"

#include "audio/plugin/VST3Plugin.hpp"
#include "util/IntegerRange.hpp"
#include "util/Util.hpp"

#include <mutex>

namespace YADAW::Audio::Host
{
constexpr auto nanosecondCount = 1000000000;

VST3ComponentHandler::VST3ComponentHandler(YADAW::Audio::Plugin::VST3Plugin& plugin):
    plugin_(&plugin),
    latencyChanged_([]() {}),
    ioChanged_([]() {}),
    parameterValueChanged_([]() {}),
    parameterInfoChanged_([]() {}),
    hostBufferIndex_{0},
    timestamp_(0),
    inputParameterChanges_{},
    outputParameterChanges_{},
    mappings_{{}, {}}
{
    plugin.setComponentHandler(*this);
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

int32 VST3ComponentHandler::doBeginEdit(int hostBufferIndex, ParamID id)
{
    // std::printf("|---doBeginEdit(%u) ", id);
    int index = -1;
    inputParameterChanges_[hostBufferIndex].addParameterData(id, index);
    // std::printf("return %d\n", index);
    return index;
}

tresult VST3ComponentHandler::doPerformEdit(int hostBufferIndex, int32 index,
    ParamValue normalizedValue, std::int64_t timestamp)
{
    // std::printf("|---doPerformEdit(%d, %lf) ", index, normalizedValue);
    int32 sampleOffset = std::round((timestamp - timestamp_) / (sampleRate() * nanosecondCount));
    int32 pointIndex = -1;
    auto ret = inputParameterChanges_[hostBufferIndex].getParameterData(index)->addPoint(sampleOffset, normalizedValue, pointIndex);
    // std::printf("return 0x%x\n", ret);
    return ret;
}

tresult VST3ComponentHandler::doEndEdit(ParamID id)
{
    // std::printf("|---doEndEdit(%u) return 0x0\n", id);
    return kResultOk;
}

tresult VST3ComponentHandler::beginEdit(ParamID id)
{
    // std::printf("beginEdit(%u)\n", id);
    auto hostBufferIndex = hostBufferIndex_.load(
        std::memory_order::memory_order_acquire);
    if(auto index = doBeginEdit(hostBufferIndex, id); index != -1)
    {
        mappings_[hostBufferIndex].emplace_back(id, index);
        return kResultOk;
    }
    return kOutOfMemory;
}

tresult VST3ComponentHandler::performEdit(ParamID id, ParamValue normalizedValue)
{
    // std::printf("performEdit(%u, %lf)\n", id, normalizedValue);
    auto hostBufferIndex = hostBufferIndex_.load(
        std::memory_order::memory_order_acquire);
    auto timestamp = YADAW::Util::currentTimeValueInNanosecond();
    // Is the following operation needed? Seems not.
    // plugin_->editController()->setParamNormalized(id, normalizedValue);
    auto iterator = std::find_if(mappings_[hostBufferIndex].begin(), mappings_[hostBufferIndex].end(),
        [id](const auto& mapping)
        {
            const auto& [mappingId, index] = mapping;
            return mappingId == id;
        }
    );
    if(iterator == mappings_[hostBufferIndex].end())
    {
        if(auto index = doBeginEdit(hostBufferIndex, id); index != -1)
        {
            if(auto performEditResult = doPerformEdit(hostBufferIndex, index, normalizedValue, timestamp);
                performEditResult == kResultOk)
            {
                return doEndEdit(id);
            }
        }
        return kResultFalse;
    }
    const auto& [mappingId, mappingIndex] = *iterator;
    return doPerformEdit(hostBufferIndex, mappingIndex, normalizedValue, timestamp);
}

tresult VST3ComponentHandler::endEdit(ParamID id)
{
    auto hostBufferIndex = hostBufferIndex_.load(
        std::memory_order::memory_order_acquire);
    // std::printf("endEdit(%u)\n", id);
    // TODO: Not sure what to do yet :(
    return doEndEdit(id);
}

tresult VST3ComponentHandler::restartComponent(int32 flags)
{
    using YADAW::Audio::Plugin::IAudioPlugin;
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
        if(status >= IAudioPlugin::Status::Activated)
        {
            plugin_->activate();
        }
        if(status >= IAudioPlugin::Status::Processing)
        {
            plugin_->startProcessing();
        }
        return kResultOk;
    }
    if(flags & RestartFlags::kIoChanged)
    {
        auto status = plugin_->status();
        plugin_->stopProcessing();
        plugin_->deactivate();
        ioChanged_();
        plugin_->activate();
        if(status >= IAudioPlugin::Status::Processing)
        {
            plugin_->startProcessing();
        }
        return kResultOk;
    }
    if(flags & RestartFlags::kLatencyChanged)
    {
        auto status = plugin_->status();
        plugin_->stopProcessing();
        plugin_->deactivate();
        plugin_->activate();
        auto latency = plugin_->audioProcessor()->getLatencySamples();
        latencyChanged_();
        if(status >= IAudioPlugin::Status::Processing)
        {
            plugin_->startProcessing();
        }
        return kResultOk;
    }
    if(flags & RestartFlags::kParamTitlesChanged)
    {
        plugin_->refreshParameterInfo();
        parameterInfoChanged_();
        return kResultOk;
    }
    if(flags & RestartFlags::kParamValuesChanged)
    {
        parameterValueChanged_();
        return kResultOk;
    }
    return kNotImplemented;
}

void VST3ComponentHandler::switchBuffer(std::int64_t switchTimestampInNanosecond)
{
    timestamp_ = switchTimestampInNanosecond;
    outputParameterChanges_[hostBufferIndex_.fetch_xor(1)].clearPointsInQueue();
    const auto hostBufferIndex = hostBufferIndex_.load(std::memory_order::memory_order_acquire);
    inputParameterChanges_[hostBufferIndex].clearPointsInQueue();
    auto pluginBufferIndex = hostBufferIndex ^ 1;
    // Add initial values before giving it to the plugin to process.
    // This is a temporary fix made for Ozone 11 Equalizer. This plugin always
    // gets the last point from the `IParamValueQueue` if the queue (returned
    // from `IParameterChanges::getParameterData`) exists, even if the queue is
    // empty, in which case `getPoint` with an index of -1 (getPointCount() - 1)
    // will be called.
    // We might like to make `IParameterChanges::getParameterData` to return
    // `nullptr` in this circumstance, which is a bad idea given that other
    // plugins might dereference the result without checking it.
    auto& inputParameterChanges = inputParameterChanges_[pluginBufferIndex];
    FOR_RANGE0(i, inputParameterChanges.getParameterCount())
    {
        auto& queue = *inputParameterChanges.getParameterData(i);
        int32 index = -1;
        queue.addPoint(0, plugin_->editController()->getParamNormalized(queue.getParameterId()), index);
    }
    plugin_->setParameterChanges(inputParameterChanges,
        outputParameterChanges_ + pluginBufferIndex);
}

void VST3ComponentHandler::consumeOutputParameterChanges(std::int64_t timestampInNanosecond)
{
    auto& outputParameterChanges = outputParameterChanges_[hostBufferIndex_.load(std::memory_order::memory_order_acquire)];
    auto* editController = plugin_->editController();
    auto outputParameterChangeCount = outputParameterChanges.getParameterCount();
    FOR_RANGE0(i, outputParameterChangeCount)
    {
        if(auto ptr = outputParameterChanges.getParameterData(i); ptr)
        {
            auto pointCount = ptr->getPointCount();
            if(pointCount != 0)
            {
                Steinberg::int32 sampleOffset = 0;
                ParamValue value = 0.0;
                if(ptr->getPoint(pointCount - 1, sampleOffset, value) == Steinberg::kResultOk)
                {
                    editController->setParamNormalized(ptr->getParameterId(), value);
                }
            }
        }
    }
}

double VST3ComponentHandler::sampleRate() const
{
    return plugin_->processSetup().sampleRate;
}

void VST3ComponentHandler::reserve()
{
    if(plugin_)
    {
        if(auto parameter = plugin_->parameter())
        {
            auto parameterCount = parameter->parameterCount();
            inputParameterChanges_[0].reserve(parameterCount);
            inputParameterChanges_[1].reserve(parameterCount);
            mappings_[0].reserve(parameterCount);
            mappings_[1].reserve(parameterCount);
        }
    }
}

void VST3ComponentHandler::latencyChanged(std::function<void()>&& callback)
{
    latencyChanged_ = std::move(callback);
}

void VST3ComponentHandler::ioChanged(std::function<void()>&& callback)
{
    ioChanged_ = std::move(callback);
}

void VST3ComponentHandler::parameterValueChanged(std::function<void()>&& callback)
{
    parameterValueChanged_ = std::move(callback);
}

void VST3ComponentHandler::parameterInfoChanged(std::function<void()>&& callback)
{
    parameterInfoChanged_ = std::move(callback);
}
}