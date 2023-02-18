#include "DAOBase.hpp"

#include "base/Constants.hpp"
#include "native/Native.hpp"

#include <QDir>
#include <QFileInfo>

namespace YADAW::DAO
{
const QString& appDataPath()
{
    static QString ret(YADAW::Native::roamingAppDataFolder() + '\\' + YADAW::Base::ProductName);
    return ret;
}

void createAppDataFolder()
{
    if(!QFileInfo::exists(appDataPath()))
    {
        QDir dir(YADAW::Native::roamingAppDataFolder());
        dir.mkdir(YADAW::Base::ProductName);
    }
}

const QString& appDatabaseFileName()
{
    static QString ret(QString(YADAW::Base::ProductName) + ".db");
    return ret;
}

const QString& appDatabasePath()
{
    static QString ret(appDataPath() + '\\' + appDatabaseFileName());
    return ret;
}

const QString& appConfigFileName()
{
    static QString ret(QString(YADAW::Base::ProductName) + ".yml");
    return ret;
}

const QString& appConfigPath()
{
    static QString ret(appDataPath() + '\\' + appConfigFileName());
    return ret;
}
}