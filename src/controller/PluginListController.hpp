#ifndef YADAW_SRC_CONTROLLER_PLUGINLISTCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINLISTCONTROLLER

#include "model/PluginListModel.hpp"

namespace YADAW::Controller
{
YADAW::Model::PluginListModel& appPluginListModel();

YADAW::Model::PluginListModel& appMIDIEffectListModel();

YADAW::Model::PluginListModel& appInstrumentListModel();

YADAW::Model::PluginListModel& appAudioEffectListModel();
}

#endif //YADAW_SRC_CONTROLLER_PLUGINLISTCONTROLLER
