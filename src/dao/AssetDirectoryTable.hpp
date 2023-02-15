#ifndef YADAW_SRC_DAO_ASSETDIRECTORYTABLE
#define YADAW_SRC_DAO_ASSETDIRECTORYTABLE

#include "dao/Database.hpp"

#include <sqlite_modern_cpp.h>

#include <vector>

namespace YADAW::DAO
{
struct AssetDirectory
{
public:
    AssetDirectory(): id(-1), path(), name() {}
    AssetDirectory(int id, const QString& path, const QString& name):
        id(id), path(path), name(name) {}
    int id;
    QString path;
    QString name;
};

void createAssetDirectoryTable(sqlite::database& database = appDatabase());

void addAssetDirectory(const QString& path, const QString& name,
    sqlite::database& database = appDatabase());

std::vector<AssetDirectory> selectAllAssetDirectories(
    sqlite::database& database = appDatabase());

AssetDirectory selectAssetDirectoryById(int id,
    sqlite::database& database = appDatabase());

void updateAssetDirectoryName(int id, const QString& name,
    sqlite::database& database = appDatabase());

void removeAssetDirectory(int id,
    sqlite::database& database = appDatabase());
}

#endif //YADAW_SRC_DAO_ASSETDIRECTORYTABLE
