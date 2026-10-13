#include "VST3ComponentHandler.hpp"

#include "audio/host/HostContext.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "util/IntegerRange.hpp"
#include "util/Base.hpp"
#include "util/Util.hpp"

#include <cinttypes>
#include <mutex>

// #define VST3_CHLOG

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
    outputParameterChanges_{}
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

std::pair<int32, VST3InputParameterChanges::ParamValueQueue*>
VST3ComponentHandler::doBeginEdit(ParamID id, std::int64_t timestampInNanosecond)
{
#ifdef VST3_CHLOG
    std::fprintf(stderr, "|---doBeginEdit(%u) ", id);
#endif
    auto [index, pvq] = inputParamChanges().emplaceParameterData(id);
#ifdef VST3_CHLOG
    std::fprintf(stderr, "return %d\n", index);
#endif
    return {index, pvq};
}

tresult VST3ComponentHandler::doPerformEdit(int32 index,
    ParamValue normalizedValue, std::int64_t timestamp)
{
    int32 sampleOffset = std::round((timestamp - timestamp_) * sampleRate() / std::nano::den);
#ifdef VST3_CHLOG
    std::fprintf(stderr, "|---doPerformEdit(%d, %lf, %" PRId64")\n"
                         "|realTimestamp = %" PRId64"\n"
                         "|timestamp_    = %" PRId64"\n"
                         "|sampleOffset  = %" PRId32"\n"
                         "|point offset  = %" PRId32"\n",
                         index, normalizedValue, timestamp,
                         YADAW::Util::currentTimeValueInNanosecond(),
                         timestamp,
                         sampleOffset,
                         sampleOffset + sampleOffset_
    );
#endif
    auto pointAdded = inputParamChanges().getParameterData(index)->addPoint(
        sampleOffset_ + sampleOffset, normalizedValue
    );
    auto ret = pointAdded? kResultOk: kOutOfMemory;
#ifdef VST3_CHLOG
    std::fprintf(stderr, "|returns %" PRIu32"\n", YADAW::Util::underlyingValue(ret));
#endif
    return ret;
}

tresult VST3ComponentHandler::doEndEdit(ParamID id, std::int64_t timestampInNanosecond)
{
#ifdef VST3_CHLOG
    std::fprintf(stderr, "|---doEndEdit(%u) return 0x0\n", id);
#endif
    editingParameters_.erase(id);
    parameterEndEditCallback_(*plugin_, id, timestampInNanosecond);
    return kResultOk;
}

tresult VST3ComponentHandler::beginEdit(ParamID id)
{
#ifdef VST3_CHLOG
    std::fprintf(stderr, "beginEdit(%u)\n", id);
#endif
    auto timestamp = YADAW::Util::currentTimeValueInNanosecond();
    auto [index, pvq] = doBeginEdit(id, timestamp);
    if(index != -1)
    {
        mappings_.emplace_back(id, index);
        editingParameters_.emplace(id, nullptr);
        return kResultOk;
    }
    return kOutOfMemory;
}

tresult VST3ComponentHandler::performEdit(ParamID id, ParamValue normalizedValue)
{
    auto timestamp = YADAW::Util::currentTimeValueInNanosecond();
#ifdef VST3_CHLOG
    std::fprintf(stderr, "performEdit(%u, %lf)\n", id, normalizedValue);
#endif
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
        mappingIndex = doBeginEdit(id, timestamp).first;
        mappings_.emplace_back(id, mappingIndex);
    }
    else
    {
        auto timer = editingIt->second.get();
        if(timer)
        {
            timer->start(std::chrono::milliseconds(500));
        }
        auto mappingIt = std::find_if(mappings_.begin(), mappings_.end(),
            [id](const auto& mapping)
            {
                const auto& [mappingId, index] = mapping;
                return mappingId == id;
            }
        );
        if(mappingIt == mappings_.end())
        {
            YADAW::Util::AlignHelper<
                YADAW::Audio::Host::VST3InputParameterChanges
            >::fromAligned(&inputParamChanges_)->emplaceParameterData(
                id
            );
            mappings_.emplace_back(id, mappingIndex);
        }
        else
        {
            mappingIndex = mappingIt->second;
        }
    }
    return doPerformEdit(mappingIndex, normalizedValue, timestamp);
}

tresult VST3ComponentHandler::endEdit(ParamID id)
{
#ifdef VST3_CHLOG
    std::fprintf(stderr, "endEdit(%u)\n", id);
#endif
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

void VST3ComponentHandler::initializeParamChanges(std::uint32_t intervalInSampleCount)
{
    auto ptr = new(YADAW::Util::AlignHelper<
        YADAW::Audio::Host::VST3InputParameterChanges
    >::fromAligned(
        &inputParamChanges_
    )) YADAW::Audio::Host::VST3InputParameterChanges(
        0, intervalInSampleCount
    );
}

void VST3ComponentHandler::uninitializeParamChanges()
{
    YADAW::Util::AlignHelper<
        YADAW::Audio::Host::VST3InputParameterChanges
    >::fromAligned(
        &inputParamChanges_
    )->~VST3InputParameterChanges();
}

void VST3ComponentHandler::bufferSwitched(
    std::int64_t switchTimestampInNanosecond,
    std::uint64_t sampleOffset)
{
#ifdef VST3_CHLOG
    std::fprintf(stderr, "VST3ComponentHandler::bufferSwitched(%" PRId64", %" PRIu64")\n", switchTimestampInNanosecond, sampleOffset);
#endif
    timestamp_ = switchTimestampInNanosecond;
    sampleOffset_ = sampleOffset;
    auto hostBufferIndex = YADAW::Audio::Host::HostContext::instance().doubleBufferSwitch.get();
    // TODO: Use this in `consumeOutputParameterChanges`
    outputParameterChanges_[hostBufferIndex].reset();
    inputParamChanges().prepareQueueForAudioThreadAndConsume(sampleOffset);
    auto pluginBufferIndex = hostBufferIndex ^ 1;
    plugin_->setParameterChanges(inputParamChanges(),
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
    if(auto parameter = plugin_->pluginParameter())
    {
        auto parameterCount = parameter->parameterCount();
        inputParamChanges().reserveParamCount(plugin_->pluginParameter()->parameterCount());
        mappings_.reserve(parameterCount);
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