#include "DatabaseController.hpp"

#include "controller/PluginDirectoryController.hpp"
#include "dao/AssetDirectoryTable.hpp"
#include "dao/DAOBase.hpp"
#include "dao/Database.hpp"
#include "dao/PluginCategoryTable.hpp"
#include "dao/PluginDirectoryTable.hpp"
#include "dao/PluginLibraryTable.hpp"
#include "dao/PluginTable.hpp"

#include <QFileInfo>

namespace YADAW::Controller
{
void initializeApplicationDatabase()
{
    using namespace YADAW::DAO;
    createAppDataFolder();
    if(!QFileInfo::exists(YADAW::DAO::appDatabasePath()))
    {
        auto database = YADAW::DAO::appDatabase();
        createAssetDirectoryTable(database);
        createPluginCategoryTable(database);
        createPluginDirectoryTable(database);
        createPluginTable(database);
        createPluginLibraryTable(database);
        YADAW::Controller::initializePluginDirectory();
    }
}
}