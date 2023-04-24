#include "VestifalPlugin.hpp"

#include "audio/host/VestifalCallback.hpp"
#include "audio/util/VestifalUtil.hpp"

namespace YADAW::Audio::Plugin
{
VestifalPlugin::VestifalPlugin()
{
}

VestifalPlugin::VestifalPlugin(VestifalEntry entry, std::int32_t uniqueId)
{
    YADAW::Audio::Host::setUniquePluginId(uniqueId);
    effect_ = entry(&YADAW::Audio::Host::vestifalHostCallback);
    if(effect_)
    {
        effect_->user = this;
        runDispatcher(effect_, EffectOpcode::effectOpen);
        status_ = Status::Created;
    }
}

VestifalPlugin::~VestifalPlugin()
{
}

AEffect* VestifalPlugin::effect()
{
    return effect_;
}

bool VestifalPlugin::initialize(double sampleRate, std::int32_t maxSampleCount)
{
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

IPluginGUI* VestifalPlugin::gui()
{
    if(effect_ && (effect_->flags & EffectFlag::effectHasEditor))
    {
        gui_ = std::make_unique<VestifalPluginGUI>(*effect_);
    }
    return gui_.get();
}

IPluginParameter* VestifalPlugin::parameter()
{
    return nullptr;
}

int VestifalPlugin::audioInputGroupCount() const
{
    return effect_->audioInputCount;
}

int VestifalPlugin::audioOutputGroupCount() const
{
    return effect_->audioOutputCount;
}

const Device::IAudioChannelGroup* VestifalPlugin::audioInputGroupAt(int index) const
{
    return nullptr;
}

const Device::IAudioChannelGroup* VestifalPlugin::audioOutputGroupAt(int index) const
{
    return nullptr;
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
}