#include "VestifalPlugin.hpp"

#include "audio/host/VestifalCallback.hpp"
#include "audio/util/VestifalHelper.hpp"

#include <mutex>

namespace YADAW::Audio::Plugin
{
// Is this needed?
// See comment of `YADAW::Audio::Host::setUniquePluginIdOnCurrentThread`
thread_local std::mutex mutex;
VestifalPlugin::VestifalPlugin()
{
}

VestifalPlugin::VestifalPlugin(VestifalEntry entry, std::int32_t uniqueId)
{
    {
        std::lock_guard<std::mutex> lg(mutex);
        YADAW::Audio::Host::setUniquePluginIdOnCurrentThread(uniqueId);
        effect_ = entry(&YADAW::Audio::Host::vestifalHostCallback);
    }
    if(effect_)
    {
        effect_->user = this;
        status_ = Status::Created;
    }
}

VestifalPlugin::~VestifalPlugin()
{
    if(status_ == Status::Processing)
    {
        VestifalPlugin::stopProcessing();
    }
    if(status_ == Status::Activated)
    {
        VestifalPlugin::deactivate();
    }
    if(status_ == Status::Initialized)
    {
        VestifalPlugin::uninitialize();
    }
    if(status_ == Status::Created)
    {
        status_ = Status::Loaded;
    }
}

AEffect* VestifalPlugin::effect()
{
    return effect_;
}

bool VestifalPlugin::initialize(double sampleRate, std::int32_t maxSampleCount)
{
    runDispatcher(effect_, EffectOpcode::effectOpen);
    runDispatcher(effect_, EffectOpcode::effectSetSampleRate, 0, 0, nullptr, sampleRate);
    runDispatcher(effect_, EffectOpcode::effectSetBlockSize, 0, maxSampleCount);
    status_ = Status::Initialized;
    inputs_.resize(effect_->audioInputCount, nullptr);
    outputs_.resize(effect_->audioOutputCount, nullptr);
    return true;
}

bool VestifalPlugin::uninitialize()
{
    gui_.reset();
    runDispatcher(effect_, EffectOpcode::effectClose);
    status_ = Status::Created;
    return true;
}

bool VestifalPlugin::activate()
{
    runDispatcher(effect_, EffectOpcode::effectMainsChanges, 0, 1);
    status_ = Status::Activated;
    return true;
}

bool VestifalPlugin::deactivate()
{
    runDispatcher(effect_, EffectOpcode::effectMainsChanges, 0, 0);
    status_ = Status::Initialized;
    return true;
}

bool VestifalPlugin::startProcessing()
{
    runDispatcher(effect_, EffectOpcode::effectStartProcessing);
    status_ = Status::Processing;
    return true;
}

bool VestifalPlugin::stopProcessing()
{
    runDispatcher(effect_, EffectOpcode::effectStopProcessing);
    status_ = Status::Activated;
    return true;
}

IAudioPlugin::Format VestifalPlugin::format()
{
    return IAudioPlugin::Format::Vestifal;
}

IAudioPlugin::Status VestifalPlugin::status()
{
    return status_;
}

std::uint32_t VestifalPlugin::tailSizeInSamples()
{
    return runDispatcher(effect_, EffectOpcode::effectGetTailSize);
}

IPluginGUI* VestifalPlugin::gui()
{
    return pluginGUI();
}

const IPluginGUI* VestifalPlugin::gui() const
{
    return pluginGUI();
}

const IPluginParameter* VestifalPlugin::parameter() const
{
    if(!parameter_)
    {
        parameter_ = std::make_unique<YADAW::Audio::Plugin::VestifalPluginParameter>(
            effect_
        );
    }
    return parameter_.get();
}

IPluginParameter* VestifalPlugin::parameter()
{
    return const_cast<IPluginParameter*>(
        static_cast<const VestifalPlugin*>(this)->parameter()
    );
}

const VestifalPluginGUI* VestifalPlugin::pluginGUI() const
{
    if(effect_ && (effect_->flags & EffectFlag::effectHasEditor))
    {
        gui_ = std::make_unique<VestifalPluginGUI>(*effect_);
    }
    return gui_.get();
}

VestifalPluginGUI* VestifalPlugin::pluginGUI()
{
    return const_cast<VestifalPluginGUI*>(
        static_cast<const VestifalPlugin&>(*this).pluginGUI()
    );
}

std::uint32_t VestifalPlugin::audioInputGroupCount() const
{
    return effect_->audioInputCount;
}

std::uint32_t VestifalPlugin::audioOutputGroupCount() const
{
    return effect_->audioOutputCount;
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    VestifalPlugin::audioInputGroupAt(std::uint32_t index) const
{
    return std::nullopt; // FIXME
}

YADAW::Audio::Device::IAudioDevice::OptionalAudioChannelGroup
    VestifalPlugin::audioOutputGroupAt(std::uint32_t index) const
{
    return std::nullopt; // FIXME
}

std::uint32_t VestifalPlugin::latencyInSamples() const
{
    return effect_->latencyInSamples;
}

void VestifalPlugin::process(const Device::AudioProcessData<float>& audioProcessData)
{
    // TODO
    effect_->processReplacing(effect_, inputs_.data(), outputs_.data(), audioProcessData.singleBufferSize);
}

std::uint32_t VestifalPlugin::audioChannelMapCount() const
{
    return 0;
}

YADAW::Audio::Device::IAudioDevice::OptionalChannelMap VestifalPlugin::audioChannelMapAt(std::uint32_t index) const
{
    return std::nullopt;
}

TimeInfo* VestifalPlugin::timeInfo() const
{
    return timeInfo_;
}

void VestifalPlugin::setTimeInfo(TimeInfo& timeInfo)
{
    timeInfo_ = &timeInfo;
}
}