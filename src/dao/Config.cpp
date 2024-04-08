#include "Config.hpp"

#include "dao/DAOBase.hpp"

#include <QFile>

namespace YADAW::DAO
{
void saveConfig(const YAML::Node& node, const QString& path)
{
    YADAW::DAO::createAppDataFolder();
    const auto& string = YAML::Dump(node);
    QFile file(path);
    file.open(QFile::OpenModeFlag::WriteOnly | QFile::OpenModeFlag::Truncate);
    file.write(string.data());
    file.close();
}

YAML::Node loadConfig(const QString& path)
{
    try
    {
        return YAML::LoadFile(path.toStdString());
    }
    catch(...)
    {
        return {};
    }
}
}