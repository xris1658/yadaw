#include "PluginTable.hpp"

#include "util/QStringUtil.hpp"

#include <string>

namespace YADAW::DAO
{
namespace Impl
{
const char16_t* createPluginTableCommand()
{
    static char16_t ret[] =
        u"CREATE TABLE IF NOT EXISTS plugin("
        "   `id`     INTEGER NOT NULL"
        "       CONSTRAINT plugin_pk"
        "           PRIMARY KEY AUTOINCREMENT,"
        "   `path`    TEXT NOT NULL,"
        "   `uid`     BLOB NOT NULL,"
        "   `name`    TEXT NOT NULL,"
        "   `vendor`  TEXT NOT NULL,"
        "   `version` TEXT NOT NULL,"
        "   `format`  INTEGER NOT NULL,"
        "   `type`    INTEGER NOT NULL"
        ");";
    return ret;
}
const char16_t* insertPluginCommand()
{
    static char16_t ret[] =
        u"INSERT INTO plugin (path, uid, name, vendor, version, format, type)"
        "VALUES (?, ?, ?, ?, ?, ?, ?)";
    return ret;
}
const char16_t* removeAllPluginsCommand()
{
    static char16_t ret[] =
        u"DELETE FROM plugin";
    return ret;
}
const char16_t* getLastPluginIdCommand()
{
    static char16_t ret[] =
        u"SELECT seq FROM sqlite_sequence WHERE name = 'plugin'";
    return ret;
}
const char16_t* resetPluginIdFromSequenceCommand()
{
    static char16_t ret[] =
        u"DELETE FROM sqlite_sequence WHERE name = 'plugin'";
    return ret;
}
const char16_t* getAllPluginCountCommand()
{
    static char16_t ret[] =
        u"SELECT COUNT(*) FROM plugin";
    return ret;
}
const char16_t* selectAllPluginCommand()
{
    static char16_t ret[] =
        u"SELECT * FROM plugin";
    return ret;
}
const char16_t* selectPluginByIdCommand()
{
    static char16_t ret[] =
        u"SELECT * FROM plugin WHERE id = ?";
    return ret;
}
const char16_t* selectPluginByTypeCommand()
{
    static char16_t ret[] =
        u"SELECT * From plugin WHERE type = ?";
    return ret;
}
const char16_t* selectPluginByFormatCommand()
{
    static char16_t ret[] =
        u"SELECT * From plugin WHERE format = ?";
    return ret;
}
const char16_t* selectCountOfPluginByPathCommand()
{
    static char16_t ret[] =
        u"SELECT COUNT(*) FROM plugin WHERE path = ?";
    return ret;
}
const char16_t* selectPluginByPathCommand()
{
    static char16_t ret[] =
        u"SELECT * FROM plugin WHERE path = ?";
    return ret;
}
const char16_t* selectCountOfDistinctPluginPathCommand()
{
    static char16_t ret[] =
        u"SELECT COUNT(DISTINCT path) FROM plugin";
    return ret;
}
const char16_t* selectDistinctPluginPathCommand()
{
    static char16_t ret[] =
        u"SELECT DISTINCT path FROM plugin";
    return ret;
}
const char16_t* removePluginByPathCommand()
{
    static char16_t ret[] =
        u"DELETE FROM plugin WHERE path = ?";
    return ret;
}
}

using namespace YADAW::Util;

void createPluginTable(sqlite::database& database)
{
    database << Impl::createPluginTableCommand();
}

int insertPlugin(const PluginInfo& pluginInfo, sqlite::database& database)
{
    database << Impl::insertPluginCommand()
        << u16DataFromQString(pluginInfo.path)
        << pluginInfo.uid
        << u16DataFromQString(pluginInfo.name)
        << u16DataFromQString(pluginInfo.vendor)
        << u16DataFromQString(pluginInfo.version)
        << pluginInfo.format
        << pluginInfo.type;
    int ret = 0;
    database << Impl::getLastPluginIdCommand() >> ret;
    return ret;
}

void removeAllPlugins(sqlite::database& database)
{
    database << Impl::removeAllPluginsCommand();
    database << Impl::resetPluginIdFromSequenceCommand();
}

std::vector<PluginInfoInDatabase> selectAllPlugin(sqlite::database& database)
{
    std::vector<PluginInfoInDatabase> ret;
    ret.reserve(getAllPluginCount());
    database << Impl::selectAllPluginCommand()
        >> [&ret](int id,
            const std::string& path,
            const std::vector<char>& uid,
            const std::string& name,
            const std::string& vendor,
            const std::string& version,
            int format,
            int type)
        {
            ret.emplace_back(id,
                QString::fromStdString(path),
                uid,
                QString::fromStdString(name),
                QString::fromStdString(vendor),
                QString::fromStdString(version),
                format,
                type);
        };
    return ret;
}

std::vector<PluginInfoInDatabase> selectPluginByType(PluginType type, sqlite::database& database)
{
    std::vector<PluginInfoInDatabase> ret;
    ret.reserve(getAllPluginCount());
    database << Impl::selectPluginByTypeCommand() << int(type)
        >> [&ret](int id,
            const std::string& path,
            const std::vector<char>& uid,
            const std::string& name,
            const std::string& vendor,
            const std::string& version,
            int format,
            int type)
        {
            ret.emplace_back(id,
                QString::fromStdString(path),
                uid,
                QString::fromStdString(name),
                QString::fromStdString(vendor),
                QString::fromStdString(version),
                format,
                type);
        };
    return ret;
}

std::vector<PluginInfoInDatabase> selectPluginByFormat(PluginFormat format, sqlite::database& database)
{
    std::vector<PluginInfoInDatabase> ret;
    ret.reserve(getAllPluginCount());
    database << Impl::selectPluginByFormatCommand() << int(format)
        >> [&ret](int id,
            const std::string& path,
            const std::vector<char>& uid,
            const std::string& name,
            const std::string& vendor,
            const std::string& version,
            int format,
            int type)
        {
            ret.emplace_back(id,
                QString::fromStdString(path),
                uid,
                QString::fromStdString(name),
                QString::fromStdString(vendor),
                QString::fromStdString(version),
                format,
                type);
         };
    return ret;
}

int getAllPluginCount(sqlite::database& database)
{
    int ret;
    database << Impl::getAllPluginCountCommand() >> ret;
    return ret;
}

PluginInfoInDatabase selectPluginById(int id, sqlite::database& database)
{
    PluginInfoInDatabase ret;
    database << Impl::selectPluginByIdCommand() << id
        >> [&ret](int id,
            const std::string& path,
            const std::vector<char>& uid,
            const std::string& name,
            const std::string& vendor,
            const std::string& version,
            int format,
            int type)
        {
            ret = {
                id,
                QString::fromStdString(path),
                uid,
                QString::fromStdString(name),
                QString::fromStdString(vendor),
                QString::fromStdString(version),
                format,
                type
            };
        };
    return ret;
}

std::vector<PluginInfoInDatabase> selectPluginByPath(const QString& path, sqlite::database& database)
{
    std::vector<PluginInfoInDatabase> ret;
    int count = 0; database << Impl::selectCountOfPluginByPathCommand() >> count;
    ret.reserve(count);
    database << Impl::selectPluginByPathCommand() << path.toStdString()
        >> [&ret](int id,
            const std::string& path,
            const std::vector<char>& uid,
            const std::string& name,
            const std::string& vendor,
            const std::string& version,
            int format,
            int type)
        {
            ret.emplace_back(
                id,
                QString::fromStdString(path),
                uid,
                QString::fromStdString(name),
                QString::fromStdString(vendor),
                QString::fromStdString(version),
                format,
                type
            );
        };
    return ret;
}

std::vector<QString> selectDistinctPluginPath(sqlite::database& database)
{
    std::vector<QString> ret;
    int count = 0; database << Impl::selectCountOfDistinctPluginPathCommand() >> count;
    ret.reserve(count);
    database << Impl::selectDistinctPluginPathCommand()
                  >> [&ret](const std::string& path)
    {
        ret.emplace_back(QString::fromStdString(path));
    };
    return ret;
}

void removePluginByPath(const QString& path, sqlite::database& database)
{
    database << Impl::removePluginByPathCommand() << path.toStdString();
}
}
