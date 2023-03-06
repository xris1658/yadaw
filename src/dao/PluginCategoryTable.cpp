#include "PluginCategoryTable.hpp"

#include "util/QStringUtil.hpp"

#include <string>

namespace YADAW::DAO
{
namespace Impl
{
const char16_t* createPluginCategoryTableCommand()
{
    static char16_t ret[] =
        u"CREATE TABLE IF NOT EXISTS plugin_category("
        "    `id`       INTEGER NOT NULL,"
        "    `category` TEXT NOT NULL"
        ");";
    return ret;
}

const char16_t* insertPluginCategoryCommand()
{
    static char16_t ret[] =
        u"INSERT INTO plugin_category (id, category)"
        "VALUES (?, ?);";
    return ret;
}

const char16_t* removeAllPluginCategoriesCommand()
{
    static char16_t ret[] =
        u"DELETE FROM plugin_category";
    return ret;
}

const char16_t* removePluginByIdCommand()
{
    static char16_t ret[] =
        u"DELETE FROM plugin_category WHERE id = ?";
    return ret;
}

const char16_t* selectByCategoryNameCommand()
{
    static char16_t ret[] =
        u"SELECT id FROM plugin_category WHERE category = ?";
    return ret;
}

const char16_t* selectCountByCategoryNameCommand()
{
    static char16_t ret[] =
        u"SELECT COUNT(id) FROM plugin_category WHERE category = ?";
    return ret;
}

const char16_t* selectByIdCommand()
{
    static char16_t ret[] =
        u"SELECT category FROM plugin_category WHERE id = ?";
    return ret;
}

const char16_t* selectCountByIdCommand()
{
    static char16_t ret[] =
        u"SELECT COUNT(category) FROM plugin_category WHERE id = ?";
    return ret;
}
}

using namespace YADAW::Util;

void createPluginCategoryTable(sqlite::database& database)
{
    database << Impl::createPluginCategoryTableCommand();
}

void insertPluginCategory(int id, const std::vector<QString>& categories, sqlite::database& database)
{
    for(auto& category: categories)
    {
        database << Impl::insertPluginCategoryCommand()
            << id
            << u16DataFromQString(category);
    }
}

void removeAllPluginCategories(sqlite::database& database)
{
    database << Impl::removeAllPluginCategoriesCommand();
}

void removePluginById(int id, sqlite::database& database)
{
    database << Impl::removePluginByIdCommand()
        << id;
}

std::vector<int> selectByCategoryName(const QString& categoryName, sqlite::database& database)
{
    std::vector<int> ret;
    int count = 0;
    database << Impl::selectCountByCategoryNameCommand() >> count;
    ret.reserve(count);
    database << Impl::selectByCategoryNameCommand() << u16DataFromQString(categoryName)
        >> [&ret](int id)
        {
            ret.emplace_back(id);
        };
    return ret;
}

std::vector<QString> selectById(int id, sqlite::database& database)
{
    std::vector<QString> ret;
    int count = 0;
    database << Impl::selectCountByIdCommand() >> count;
    ret.reserve(count);
    database << Impl::selectByIdCommand() << id
        >> [&ret](const std::u16string& category)
        {
            ret.emplace_back(QString::fromStdU16String(category));
        };
    return ret;
}
}