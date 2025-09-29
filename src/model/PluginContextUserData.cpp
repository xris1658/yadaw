#include "PluginContextUserData.hpp"

namespace YADAW::Model
{
PluginContextUserData::PluginContextUserData(
    const YADAW::Controller::PluginContext& pluginContext, const QString& name):
    paramListModel(*(pluginContext.pluginInstance.plugin()->get().parameter())),
    audioInputs(pluginContext.pluginInstance.plugin()->get(), true),
    audioOutputs(pluginContext.pluginInstance.plugin()->get(), false),
    name(name)
{}

void PluginContextUserData::initAuxModels(
    std::optional<std::uint32_t> mainAudioInput,
    std::optional<std::uint32_t> mainAudioOutput)
{
    if(mainAudioInput)
    {
        audioAuxInputs = std::make_unique<
            YADAW::Model::AudioDeviceAuxIOGroupListModel
        >(audioInputs, *mainAudioInput);
    }
    if(mainAudioOutput)
    {
        audioAuxOutputs = std::make_unique<
            YADAW::Model::AudioDeviceAuxIOGroupListModel
        >(audioOutputs, *mainAudioOutput);
    }
}

const YADAW::Model::IAudioDeviceIOGroupListModel& PluginContextUserData::getAudioAuxInputs() const
{
    if(audioAuxInputs)
    {
        return *audioAuxInputs;
    }
    return audioInputs;
}

const YADAW::Model::IAudioDeviceIOGroupListModel& PluginContextUserData::getAudioAuxOutputs() const
{
    if(audioAuxOutputs)
    {
        return *audioAuxOutputs;
    }
    return audioOutputs;
}

YADAW::Model::IAudioDeviceIOGroupListModel& PluginContextUserData::getAudioAuxInputs()
{
    return const_cast<YADAW::Model::IAudioDeviceIOGroupListModel&>(
        static_cast<const PluginContextUserData&>(*this).getAudioAuxInputs()
    );
}

YADAW::Model::IAudioDeviceIOGroupListModel& PluginContextUserData::getAudioAuxOutputs()
{
    return const_cast<YADAW::Model::IAudioDeviceIOGroupListModel&>(
        static_cast<const PluginContextUserData&>(*this).getAudioAuxOutputs()
    );
}

namespace Impl
{
bool blankFillPluginContext(
    YADAW::Controller::PluginContext& context,
    const YADAW::Controller::InitPluginArgs& initPluginArgs)
{
    return false;
}
}
}
