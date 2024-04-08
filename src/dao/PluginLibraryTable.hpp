#ifndef YADAW_SRC_DAO_PLUGINLIBRARYTABLE
#define YADAW_SRC_DAO_PLUGINLIBRARYTABLE

#include "dao/Database.hpp"

#include <sqlite_modern_cpp.h>

#include <QString>

#include <vector>

namespace YADAW::DAO
{
struct PluginLibraryInfo
{
    PluginLibraryInfo(
        const QString& path, const std::vector<char>& crc32, int rescan, int format):
        path(path), crc32(crc32), rescan(rescan), format(format)
    {}
    PluginLibraryInfo(const PluginLibraryInfo&) = default;
    PluginLibraryInfo(PluginLibraryInfo&&) = default;
    ~PluginLibraryInfo() = default;
    QString path;
    std::vector<char> crc32;
    int rescan;
    int format;
};

struct PluginLibraryInfoInDatabase
{
    PluginLibraryInfoInDatabase(int id, const QString& path,
        const std::vector<char>& crc32, int rescan, int format):
        id(id), path(path), crc32(crc32), rescan(rescan), format(format)
    {}
    PluginLibraryInfoInDatabase(const PluginLibraryInfoInDatabase&) = default;
    PluginLibraryInfoInDatabase(PluginLibraryInfoInDatabase&&) = default;
    ~PluginLibraryInfoInDatabase() = default;
    int id;
    QString path;
    std::vector<char> crc32;
    int rescan;
    int format;
};

void createPluginLibraryTable(sqlite::database& database = appDatabase());

int addPluginLibrary(const PluginLibraryInfo& info, sqlite::database& database = appDatabase());

std::vector<PluginLibraryInfoInDatabase> selectAllPluginLibraries(
    sqlite::database& database = appDatabase()
);

std::vector<PluginLibraryInfoInDatabase> selectRescanPluginLibraries(
    bool rescan,
    sqlite::database& database = appDatabase()
);

void removePluginLibraryById(int id, sqlite::database& database = appDatabase());

void removeAllPluginLibraries(sqlite::database& database = appDatabase());
}

#endif // YADAW_SRC_DAO_PLUGINLIBRARYTABLE
