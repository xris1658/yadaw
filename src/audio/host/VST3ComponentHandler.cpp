#include "VST3ComponentHandler.hpp"

#include "audio/host/HostContext.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "util/IntegerRange.hpp"
#include "util/Util.hpp"

#include <mutex>

void blankLatencyChangedCallback(YADAW::Audio::Plugin::VST3Plugin&) {}

void blankIOChangedCallback(YADAW::Audio::Plugin::VST3Plugin&) {}

void blankParameterValueChangedCallback(YADAW::Audio::Plugin::VST3Plugin&) {}

void blankParameterInfoChangedCallback(YADAW::Audio::Plugin::VST3Plugin&) {}

void blankParameterBeginEditCallback(
    YADAW::Audio::Plugin::VST3Plugin&,
    Steinberg::Vst::ParamID,
    std::int64_t)
{}

void blankParameterPerformEditCallback(
    YADAW::Audio::Plugin::VST3Plugin&,
    Steinberg::Vst::ParamID, Steinberg::Vst::ParamValue,
    std::int64_t)
{}

void blankParameterEndEditCallback(
    YADAW::Audio::Plugin::VST3Plugin&,
    Steinberg::Vst::ParamID,
    std::int64_t)
{}

namespace YADAW::Audio::Host
{
constexpr auto nanosecondCount = 1000000000;

VST3ComponentHandler::VST3ComponentHandler(YADAW::Audio::Plugin::VST3Plugin& plugin):
    plugin_(&plugin),
    latencyChangedCallback_(&blankLatencyChangedCallback),
    ioChangedCallback_(&blankIOChangedCallback),
    parameterValueChangedCallback_(&blankParameterValueChangedCallback),
    parameterInfoChangedCallback_(&blankParameterInfoChangedCallback),
    parameterBeginEditCallback_(&blankParameterBeginEditCallback),
    parameterPerformEditCallback_(&blankParameterPerformEditCallback),
    parameterEndEditCallback_(&blankParameterEndEditCallback),
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

int32 VST3ComponentHandler::doBeginEdit(int hostBufferIndex, ParamID id, std::int64_t timestampInNanosecond)
{
    // std::printf("|---doBeginEdit(%u) ", id);
    int index = -1;
    inputParameterChanges_[hostBufferIndex].addParameterData(id, index);
    if(index != -1)
    {
        parameterBeginEditCallback_(*plugin_, id, timestampInNanosecond);
    }
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

tresult VST3ComponentHandler::doEndEdit(ParamID id, std::int64_t timestampInNanosecond)
{
    // std::printf("|---doEndEdit(%u) return 0x0\n", id);
    editingParameters_.erase(id);
    parameterEndEditCallback_(*plugin_, id, timestampInNanosecond);
    return kResultOk;
}

tresult VST3ComponentHandler::beginEdit(ParamID id)
{
    // std::printf("beginEdit(%u)\n", id);
    auto hostBufferIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    auto timestamp = YADAW::Util::currentTimeValueInNanosecond();
    if(auto index = doBeginEdit(hostBufferIndex, id, timestamp); index != -1)
    {
        mappings_[hostBufferIndex].emplace_back(id, index);
        editingParameters_.emplace(id, nullptr);
        return kResultOk;
    }
    return kOutOfMemory;
}

tresult VST3ComponentHandler::performEdit(ParamID id, ParamValue normalizedValue)
{
    auto hostBufferIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    auto timestamp = YADAW::Util::currentTimeValueInNanosecond();
    auto editingIt = editingParameters_.find(id);
    int32 mappingIndex = -1;
    if(editingIt == editingParameters_.end())
    {
        // Wheel event without `beginEdit` and `endEdit`
        std::unique_ptr<QTimer> timer;
        if(availableTimers_.empty())
        {
            timer = std::make_unique<QTimer>();
            timer->setSingleShot(true);
        }
        else
        {
            timer = std::move(*availableTimers_.begin());
            availableTimers_.erase_after(availableTimers_.before_begin());
        }
        editingIt = editingParameters_.emplace(id, std::move(timer)).first;
        QObject::connect(editingIt->second.get(), &QTimer::timeout,
            [this, id, editingIt]()
            {
                auto& timer = editingIt->second;
                QObject::disconnect(timer.get(), &QTimer::timeout, nullptr, nullptr);
                availableTimers_.emplace_front(std::move(timer));
                doEndEdit(id, YADAW::Util::currentTimeValueInNanosecond());
            }
        );
        mappingIndex = doBeginEdit(hostBufferIndex, id, timestamp);
    }
    else
    {
        auto timer = editingIt->second.get();
        if(timer)
        {
            timer->start(std::chrono::milliseconds(500));
        }
        auto& mapping = mappings_[hostBufferIndex];
        auto mappingIt = std::find_if(mapping.begin(), mapping.end(),
            [id](const auto& mapping)
            {
                const auto& [mappingId, index] = mapping;
                return mappingId == id;
            }
        );
        if(mappingIt == mapping.end())
        {
            inputParameterChanges_[hostBufferIndex].addParameterData(id, mappingIndex);
            mapping.emplace_back(id, mappingIndex);
        }
        else
        {
            mappingIndex = mappingIt->second;
        }
    }
    return doPerformEdit(hostBufferIndex, mappingIndex, normalizedValue, timestamp);    
}

tresult VST3ComponentHandler::endEdit(ParamID id)
{
    // std::printf("endEdit(%u)\n", id);
    auto timestamp = YADAW::Util::currentTimeValueInNanosecond();
    return doEndEdit(id, timestamp);
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
        ioChangedCallback_(*plugin_);
        // Some plugins might request a restart even it is not activated.
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
    if(flags & RestartFlags::kLatencyChanged)
    {
        auto status = plugin_->status();
        plugin_->stopProcessing();
        plugin_->deactivate();
        // Some plugins might request a restart even it is not activated.
        if(status >= IAudioPlugin::Status::Activated)
        {
            plugin_->activate();
            latencyChangedCallback_(*plugin_);
        }
        if(status >= IAudioPlugin::Status::Processing)
        {
            plugin_->startProcessing();
        }
        return kResultOk;
    }
    if(flags & RestartFlags::kParamTitlesChanged)
    {
        plugin_->refreshParameterInfo();
        parameterInfoChangedCallback_(*plugin_);
        return kResultOk;
    }
    if(flags & RestartFlags::kParamValuesChanged)
    {
        parameterValueChangedCallback_(*plugin_);
        return kResultOk;
    }
    return kNotImplemented;
}

void VST3ComponentHandler::bufferSwitched(std::int64_t switchTimestampInNanosecond)
{
    timestamp_ = switchTimestampInNanosecond;
    auto oldHostBufferIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    outputParameterChanges_[oldHostBufferIndex].clearPointsInQueue();
    const auto hostBufferIndex = oldHostBufferIndex ^ 1;
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
    auto& outputParameterChanges = outputParameterChanges_[YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get()];
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

void VST3ComponentHandler::setLatencyChangedCallback(
    VST3ComponentHandler::LatencyChangedCallback* callback)
{
    latencyChangedCallback_ = callback;
}

void VST3ComponentHandler::setIOChangedCallback(
    VST3ComponentHandler::IOChangedCallback* callback)
{
    ioChangedCallback_ = callback;
}

void VST3ComponentHandler::setParameterValueChangedCallback(
    VST3ComponentHandler::ParameterValueChangedCallback* callback)
{
    parameterValueChangedCallback_ = callback;
}

void VST3ComponentHandler::setParameterInfoChangedCallback(
    VST3ComponentHandler::ParameterInfoChangedCallback* callback)
{
    parameterInfoChangedCallback_ = callback;
}

void VST3ComponentHandler::setParameterBeginEditCallback(
    VST3ComponentHandler::ParameterBeginEditCallback* callback)
{
    parameterBeginEditCallback_ = callback;
}
void VST3ComponentHandler::setParameterPerformEditCallback(
    VST3ComponentHandler::ParameterPerformEditCallback* callback)
{
    parameterPerformEditCallback_ = callback;
}
void VST3ComponentHandler::setParameterEndEditCallback(
    VST3ComponentHandler::ParameterEndEditCallback* callback)
{
    parameterEndEditCallback_ = callback;
}

void VST3ComponentHandler::resetLatencyChangedCallback()
{
    latencyChangedCallback_ = &blankLatencyChangedCallback;
}

void VST3ComponentHandler::resetIOChangedCallback()
{
    ioChangedCallback_ = &blankIOChangedCallback;
}

void VST3ComponentHandler::resetParameterValueChangedCallback()
{
    parameterValueChangedCallback_ = &blankParameterValueChangedCallback;
}

void VST3ComponentHandler::resetParameterInfoChangedCallback()
{
    parameterInfoChangedCallback_ = &blankParameterInfoChangedCallback;
}

void VST3ComponentHandler::resetParameterBeginEditCallback()
{
    parameterBeginEditCallback_ = &blankParameterBeginEditCallback;
}

void VST3ComponentHandler::resetParameterPerformEditCallback()
{
    parameterPerformEditCallback_ = &blankParameterPerformEditCallback;
}

void VST3ComponentHandler::resetParameterEndEditCallback()
{
    parameterEndEditCallback_ = &blankParameterEndEditCallback;
}
}