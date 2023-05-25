#include "AssetDirectoryTable.hpp"

#include "util/QStringUtil.hpp"

namespace YADAW::DAO
{
namespace Impl
{
const char16_t* createAssetDirectoryTableCommand()
{
    static char16_t ret[] =
        u"CREATE TABLE IF NOT EXISTS asset_directory("
        "   `id` INTEGER NOT NULL"
        "       CONSTRAINT id_pk"
        "           PRIMARY KEY AUTOINCREMENT,"
        "   `path` TEXT NOT NULL,"
        "   `name` TEXT NOT NULL"
        ");";
    return ret;
}

const char16_t* addAssetDirectoryCommand()
{
    static char16_t ret[] =
        u"INSERT INTO asset_directory (path, name)"
        "VALUES(?, ?)";
    return ret;
}

const char16_t* getAssetDirectoryLastIdCommand()
{
    static char16_t ret[] =
        u"SELECT seq FROM sqlite_sequence WHERE name = \'asset_directory\'";
    return ret;
}

const char16_t* selectAllAssetDirectoryCommand()
{
    static char16_t ret[] =
        u"SELECT * FROM asset_directory";
    return ret;
}

const char16_t* getAllAssetDirectoryCountCommand()
{
    static char16_t ret[] =
        u"SELECT COUNT(*) FROM asset_directory";
    return ret;
}

const char16_t* selectAssetDirectoryByIdCommand()
{
    static char16_t ret[] =
        u"SELECT * FROM asset_directory WHERE id = ?";
    return ret;
}

const char16_t* updateAssetDirectoryNameByIdCommand()
{
    static char16_t ret[] =
        u"UPDATE asset_directory SET name = ? WHERE id = ?";
    return ret;
}

const char16_t* removeAssetDirectoryByIdCommand()
{
    static char16_t ret[] =
        u"DELETE FROM asset_directory WHERE id = ?";
    return ret;
}
}

using namespace YADAW::Util;

void createAssetDirectoryTable(sqlite::database& database)
{
    database << Impl::createAssetDirectoryTableCommand();
}

int addAssetDirectory(const QString& path, const QString& name, sqlite::database& database)
{
    database << Impl::addAssetDirectoryCommand()
        << u16DataFromQString(path) << u16DataFromQString(name);
    int ret = 0;
    database << Impl::getAssetDirectoryLastIdCommand() >> ret;
    return ret;
}

std::vector<AssetDirectory> selectAllAssetDirectories(sqlite::database& database)
{
    std::vector<AssetDirectory> ret;
    int count = 0;
    database << Impl::getAllAssetDirectoryCountCommand() >> count;
    ret.reserve(count);
    database << Impl::selectAllAssetDirectoryCommand()
    >> [&ret](int id, const std::string& path, const std::string& name)
    {
        ret.emplace_back(id, QString::fromUtf8(path), QString::fromUtf8(name));
    };
    return ret;
}

AssetDirectory selectAssetDirectoryById(int id, sqlite::database& database)
{
    AssetDirectory ret;
    database << Impl::selectAssetDirectoryByIdCommand() << id
    >> [&ret](int id, const std::string& path, const std::string& name)
    {
        ret = AssetDirectory{id, QString::fromStdString(path), QString::fromStdString(name)};
    };
    return ret;
}

void updateAssetDirectoryName(int id, const QString& name, sqlite::database& database)
{
    database << Impl::updateAssetDirectoryNameByIdCommand() << u16DataFromQString(name) << id;
}

void removeAssetDirectory(int id, sqlite::database& database)
{
    database << Impl::removeAssetDirectoryByIdCommand() << id;
}
}