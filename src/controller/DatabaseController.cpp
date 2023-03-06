#include "DatabaseController.hpp"

#include "controller/PluginDirectoryController.hpp"
#include "dao/DAOBase.hpp"
#include "dao/Database.hpp"

#include <QFileInfo>

namespace YADAW::Controller
{
void initializeApplicationDatabase()
{
    YADAW::DAO::createAppDataFolder();
    if(!QFileInfo::exists(YADAW::DAO::appDatabasePath()))
    {
        YADAW::DAO::initDatabase(YADAW::DAO::appDatabase());
        YADAW::Controller::initializePluginDirectory();
    }
}
}