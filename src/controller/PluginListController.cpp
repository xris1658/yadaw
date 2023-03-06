#include "PluginListController.hpp"

#include "dao/PluginTable.hpp"
#include "model/PluginListModelImpl.hpp"

namespace YADAW::Controller
{
YADAW::Model::PluginListModel& appPluginListModel()
{
    static YADAW::Model::PluginListModelImpl ret(
        []() { return YADAW::DAO::selectAllPlugin(); });
    return ret;
}

YADAW::Model::PluginListModel& appMIDIEffectListModel()
{
    static YADAW::Model::PluginListModelImpl ret(
        []() { return YADAW::DAO::selectPluginByType(YADAW::DAO::PluginTypeMIDIEffect); });
    return ret;
}

YADAW::Model::PluginListModel& appInstrumentListModel()
{
    static YADAW::Model::PluginListModelImpl ret(
        []() { return YADAW::DAO::selectPluginByType(YADAW::DAO::PluginTypeInstrument); });
    return ret;
}

YADAW::Model::PluginListModel& appAudioEffectListModel()
{
    static YADAW::Model::PluginListModelImpl ret(
        []() { return YADAW::DAO::selectPluginByType(YADAW::DAO::PluginTypeAudioEffect); });
    return ret;
}
}