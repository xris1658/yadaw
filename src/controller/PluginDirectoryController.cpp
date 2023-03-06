#include "PluginDirectoryController.hpp"

#include "dao/PluginDirectoryTable.hpp"
#include "model/PluginDirectoryListModelImpl.hpp"
#include "native/Native.hpp"

namespace YADAW::Controller
{
YADAW::Model::PluginDirectoryListModel& appPluginDirectoryListModel()
{
    static YADAW::Model::PluginDirectoryListModelImpl ret;
    return ret;
}

void initializePluginDirectory()
{
    const auto& list = YADAW::Native::defaultPluginDirectoryList();
    for(const auto& dir: list)
    {
        YADAW::DAO::addPluginDirectory(dir);
    }
}
}