#include "VST3ComponentHandler.hpp"

namespace YADAW::Audio::Host
{
VST3ComponentHandler::VST3ComponentHandler(YADAW::Audio::Plugin::VST3Plugin* plugin):
    plugin_(plugin)
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

tresult VST3ComponentHandler::beginEdit(ParamID id)
{
    return kResultOk;
}

tresult VST3ComponentHandler::performEdit(ParamID id, ParamValue normalizedValue)
{
    return kResultOk;
}

tresult VST3ComponentHandler::endEdit(ParamID id)
{
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
}