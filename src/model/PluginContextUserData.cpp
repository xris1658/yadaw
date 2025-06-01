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
}
