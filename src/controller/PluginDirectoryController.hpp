#ifndef YADAW_SRC_CONTROLLER_PLUGINDIRECTORYCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINDIRECTORYCONTROLLER

#include "model/PluginDirectoryListModel.hpp"

#include <vector>

namespace YADAW::Controller
{
Model::PluginDirectoryListModel& appPluginDirectoryListModel();

void initializePluginDirectory();
}

#endif //YADAW_SRC_CONTROLLER_PLUGINDIRECTORYCONTROLLER
