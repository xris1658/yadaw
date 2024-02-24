#include "PluginListController.hpp"

#include "dao/PluginTable.hpp"
#include "model/PluginListModel.hpp"

namespace YADAW::Controller
{
YADAW::Model::PluginListModel& appPluginListModel()
{
    static YADAW::Model::PluginListModel ret(
        []() { return YADAW::DAO::selectAllPlugin(); });
    return ret;
}

}