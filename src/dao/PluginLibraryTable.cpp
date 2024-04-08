#include "PluginLibraryTable.hpp"

#include <util/QStringUtil.hpp>

namespace YADAW::DAO
{
namespace Impl
{
const char16_t createPluginLibraryTableCommand[] =
    u"CREATE TABLE IF NOT EXISTS plugin_library("
    "    `id` INTEGER NOT NULL"
    "        CONSTRAINT plugin_libraries_pk"
    "            PRIMARY KEY AUTOINCREMENT,"
    "    `path` TEXT NOT NULL,"
    "    `crc32` BLOB NOT NULL,"
    "    `rescan` INTEGER NOT NULL,"
    "    `format` INTEGER NOT NULL"
    ")";

const char16_t addPluginLibraryCommand[] =
    u"INSERT INTO plugin_library (path, crc32, rescan, format)"
    "VALUES (?, ?, ?, ?)";

const char16_t getLastIdCommand[] =
    u"SELECT seq FROM sqlite_sequence WHERE name = 'plugin_library'";

const char16_t resetLastIdCommand[] =
    u"DELETE FROM sqlite_sequence WHERE name = 'plugin_library'";

const char16_t selectAllPluginLibrariesCommand[] =
    u"SELECT * FROM plugin_library";

const char16_t getAllPluginLibraryCountCommand[] =
    u"SELECT COUNT(*) FROM plugin_library";

const char16_t selectRescanPluginLibrariesCommand[] =
    u"SELECT * FROM plugin_library WHERE rescan = ?";

const char16_t selectRescanPluginLibraryCountCommand[] =
    u"SELECT COUNT(*) FROM plugin_library WHERE rescan = ?";

const char16_t removePluginLibraryByIdCommand[] =
    u"DELETE FROM plugin_library WHERE id = ?";

const char16_t removeAllPluginLibrariesCommand[] =
    u"DELETE FROM plugin_library";
}

void createPluginLibraryTable(sqlite::database& database)
{
    database << Impl::createPluginLibraryTableCommand;
}

int addPluginLibrary(const PluginLibraryInfo& info, sqlite::database& database)
{
    database << Impl::addPluginLibraryCommand
        << YADAW::Util::u16DataFromQString(info.path)
        << info.crc32
        << info.rescan
        << info.format;
    int ret = 0;
    database << Impl::getLastIdCommand >> ret;
    return ret;
}

int getAllPluginLibraryCount(sqlite::database& database)
{
    int ret;
    database << Impl::getAllPluginLibraryCountCommand >> ret;
    return ret;
}

std::vector<PluginLibraryInfoInDatabase> selectAllPluginLibraries(sqlite::database& database)
{
    std::vector<PluginLibraryInfoInDatabase> ret;
    ret.reserve(getAllPluginLibraryCount(database));
    database << Impl::selectAllPluginLibrariesCommand
        >> [&ret](int id,
            const std::string& path,
            const std::vector<char>& crc32,
            int rescan,
            int format)
        {
            ret.emplace_back(
                id, QString::fromStdString(path), crc32, rescan, format
            );
        };
    return ret;
}

int getRescanPluginLibraryCount(bool rescan, sqlite::database& database)
{
    int ret;
    database << Impl::selectRescanPluginLibraryCountCommand
        << static_cast<int>(rescan) >> ret;
    return ret;
}

std::vector<PluginLibraryInfoInDatabase> selectRescanPluginLibraries(
    bool rescan, sqlite::database& database)
{
    std::vector<PluginLibraryInfoInDatabase> ret;
    ret.reserve(getRescanPluginLibraryCount(rescan, database));
    database << Impl::selectRescanPluginLibrariesCommand << static_cast<int>(rescan)
        >> [&ret](int id,
            const std::string& path,
            const std::vector<char>& crc32,
            int rescan,
            int format)
        {
            ret.emplace_back(
                id, QString::fromStdString(path), crc32, rescan, format
            );
        };
    return ret;
}

void removePluginLibraryById(int id, sqlite::database& database)
{
    database << Impl::removePluginLibraryByIdCommand << id;
}

void removeAllPluginLibraries(sqlite::database& database)
{
    database << Impl::removeAllPluginLibrariesCommand;
    database << Impl::resetLastIdCommand;
}
}