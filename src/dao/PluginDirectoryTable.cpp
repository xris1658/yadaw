#include "PluginDirectoryTable.hpp"

namespace YADAW::DAO
{
namespace Impl
{
const char16_t* createPluginDirectoryTableCommand()
{
    static char16_t ret[] =
        u"CREATE TABLE IF NOT EXISTS plugin_directory("
        "   `path` TEXT NOT NULL"
        ");";
    return ret;
}
const char16_t* addPluginDirectoryCommand()
{
    static char16_t ret[] =
        u"INSERT INTO plugin_directory (path)"
        "VALUES(?)";
    return ret;
}
const char16_t* removePluginDirectoryCommand()
{
    static char16_t ret[] =
        u"DELETE FROM plugin_directory WHERE path = ?";
    return ret;
}
const char16_t* selectPluginDirectoryCommand()
{
    static char16_t ret[] =
        u"SELECT * FROM plugin_directory";
    return ret;
}

const char16_t* selectPluginDirectoryCountCommand()
{
    static char16_t ret[] =
        u"SELECT COUNT(*) FROM plugin_directory";
    return ret;
}
}

void createPluginDirectoryTable(sqlite::database& database)
{
    database << Impl::createPluginDirectoryTableCommand();
}

void addPluginDirectory(const QString& directory, sqlite::database& database)
{
    database << Impl::addPluginDirectoryCommand()
                  << directory.toStdU16String();
}

void removePluginDirectory(const QString& directory, sqlite::database& database)
{
    database << Impl::removePluginDirectoryCommand()
                  << directory.toStdU16String();
}

std::vector<QString> selectPluginDirectory(sqlite::database& database)
{
    std::vector<QString> ret;
    int size;
    database << Impl::selectPluginDirectoryCountCommand() >> size;
    ret.reserve(size);
    database << Impl::selectPluginDirectoryCommand()
                  >> [&ret](const std::u16string& item)
    {
        ret.emplace_back(QString::fromStdU16String(item));
    };
    return ret;
}
}
