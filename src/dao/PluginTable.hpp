#ifndef YADAW_DAO_PLUGINDAO
#define YADAW_DAO_PLUGINDAO

#include "dao/Database.hpp"

#include <sqlite_modern_cpp.h>

#include <QString>

#include <vector>

namespace YADAW::DAO
{
enum PluginFormat: int
{
    PluginFormatUnknown,
    PluginFormatVST3,
    PluginFormatCLAP
};

enum PluginType: int
{
    PluginTypeUnknown,
    PluginTypeInstrument,
    PluginTypeAudioEffect,
    PluginTypeMIDIEffect
};

struct PluginInfo
{
    PluginInfo():
        path(), uid(), name(), vendor(), version(), format(-1), type(-1)
    {}

    PluginInfo(const QString& path, const std::vector<char>& uid, const QString& name, const QString& vendor,
        const QString& version, int format, int type):
        path(path), uid(uid), name(name), vendor(vendor), version(version), format(format), type(type)
    {}
    QString path;
    std::vector<char> uid;
    QString name;
    QString vendor;
    QString version;
    int format;
    int type;
};

struct PluginInfoInDatabase
{
    PluginInfoInDatabase():
        id(-1), path(), uid(), name(), vendor(), version(), format(-1), type(-1)
    {}

    PluginInfoInDatabase(int id, const QString& path, const std::vector<char>& uid, const QString& name,
        const QString& vendor, const QString& version, int format, int type):
        id(id), path(path), uid(uid), name(name), vendor(vendor), version(version), format(format), type(type)
    {}
    int id;
    QString path;
    std::vector<char> uid;
    QString name;
    QString vendor;
    QString version;
    int format;
    int type;
};

void createPluginTable(sqlite::database& database = appDatabase());

void insertPlugin(const PluginInfo& pluginWriteInfo, sqlite::database& database = appDatabase());

void removeAllPlugins(sqlite::database& database = appDatabase());

std::vector<PluginInfoInDatabase> selectAllPlugin(sqlite::database& database = appDatabase());

int getAllPluginCount(sqlite::database& database = appDatabase());

PluginInfoInDatabase selectPluginById(int id, sqlite::database& database = appDatabase());

std::vector<PluginInfoInDatabase> selectPluginByPath(const QString& path, sqlite::database& database = appDatabase());

std::vector<QString> selectDistinctPluginPath(sqlite::database& database = appDatabase());

void removePluginByPath(const QString& path, sqlite::database& database = appDatabase());
}

#endif // YADAW_DAO_PLUGINDAO
