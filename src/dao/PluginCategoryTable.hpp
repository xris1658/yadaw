#ifndef YADAW_SRC_DAO_PLUGINCATEGORYTABLE
#define YADAW_SRC_DAO_PLUGINCATEGORYTABLE

#include "dao/Database.hpp"

#include <sqlite_modern_cpp.h>

#include <QString>

#include <vector>

namespace YADAW::DAO
{
struct PluginCategoryInfo
{
    PluginCategoryInfo():
        id(-1), category()
    {}
    PluginCategoryInfo(int id, const QString& category):
        id(id), category(category)
{}
    int id;
    QString category;
};

void createPluginCategoryTable(sqlite::database& database = appDatabase());

void insertPluginCategory(int id, const std::vector<QString>& categories, sqlite::database& database = appDatabase());

void removeAllPluginCategories(sqlite::database& database = appDatabase());

void removePluginById(int id, sqlite::database& database = appDatabase());

std::vector<int> selectByCategoryName(const QString& categoryName, sqlite::database& database = appDatabase());

std::vector<QString> selectById(int id, sqlite::database& database = appDatabase());
}

#endif // YADAW_SRC_DAO_PLUGINCATEGORYTABLE
