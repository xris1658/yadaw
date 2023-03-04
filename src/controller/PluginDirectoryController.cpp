#include "PluginDirectoryController.hpp"

#include "model/PluginDirectoryListModelImpl.hpp"

namespace YADAW::Controller
{
YADAW::Model::PluginDirectoryListModel& appPluginDirectoryListModel()
{
    static YADAW::Model::PluginDirectoryListModelImpl ret;
    return ret;
}
}