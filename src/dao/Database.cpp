#include "Database.hpp"

#include "base/Constants.hpp"
#include "dao/DAOBase.hpp"
#include "dao/AssetDirectoryTable.hpp"
#include "dao/PluginCategoryTable.hpp"
#include "dao/PluginDirectoryTable.hpp"
#include "dao/PluginTable.hpp"

#include <sqlite_modern_cpp.h>

namespace YADAW::DAO
{
sqlite::database& appDatabase()
{
    static sqlite::database ret(appDatabasePath().toStdString());
    return ret;
}

void initDatabase(sqlite::database& database)
{
    createAssetDirectoryTable(database);
    createPluginCategoryTable(database);
    createPluginDirectoryTable(database);
    createPluginTable(database);
}
}