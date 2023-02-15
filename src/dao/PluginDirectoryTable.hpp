#ifndef YADAW_DAO_PLUGINDIRECTORYDAO
#define YADAW_DAO_PLUGINDIRECTORYDAO

#include "dao/Database.hpp"

#include <sqlite_modern_cpp.h>

#include <QString>

#include <vector>

namespace YADAW::DAO
{
void createPluginDirectoryTable(sqlite::database& database = appDatabase());

void addPluginDirectory(const QString& directory, sqlite::database& database = appDatabase());

void removePluginDirectory(const QString& directory, sqlite::database& database = appDatabase());

std::vector<QString> selectPluginDirectory(sqlite::database& database = appDatabase());
}

#endif // YADAW_DAO_PLUGINDIRECTORYDAO
