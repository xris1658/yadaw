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

YADAW::Model::PluginListModel& appMIDIEffectListModel()
{
    static YADAW::Model::PluginListModel ret(
        []() { return YADAW::DAO::selectPluginByType(YADAW::DAO::PluginTypeMIDIEffect); });
    return ret;
}

YADAW::Model::PluginListModel& appInstrumentListModel()
{
    static YADAW::Model::PluginListModel ret(
        []() { return YADAW::DAO::selectPluginByType(YADAW::DAO::PluginTypeInstrument); });
    return ret;
}

YADAW::Model::PluginListModel& appAudioEffectListModel()
{
    static YADAW::Model::PluginListModel ret(
        []() { return YADAW::DAO::selectPluginByType(YADAW::DAO::PluginTypeAudioEffect); });
    return ret;
}

YADAW::Model::PluginListModel& appVestifalPluginListModel()
{
    static YADAW::Model::PluginListModel ret(
        []() { return YADAW::DAO::selectPluginByFormat(YADAW::DAO::PluginFormatVestifal); });
    return ret;
}

YADAW::Model::PluginListModel& appVST3PluginListModel()
{
    static YADAW::Model::PluginListModel ret(
        []() { return YADAW::DAO::selectPluginByFormat(YADAW::DAO::PluginFormatVST3); });
    return ret;
}

YADAW::Model::PluginListModel& appCLAPPluginListModel()
{
    static YADAW::Model::PluginListModel ret(
        []() { return YADAW::DAO::selectPluginByFormat(YADAW::DAO::PluginFormatCLAP); });
    return ret;
}
}