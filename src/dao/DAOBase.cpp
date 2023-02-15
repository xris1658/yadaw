#include "DAOBase.hpp"

#include "base/Constants.hpp"
#include "native/Native.hpp"

namespace YADAW::DAO
{
const QString& appDataPath()
{
    static QString ret(YADAW::Native::roamingAppDataFolder() + '\\' + YADAW::Base::ProductName);
    return ret;
}

const QString& appDatabasePath()
{
    static QString ret(appDataPath() + '\\' + YADAW::Base::ProductName + ".db");
    return ret;
}

const QString& appConfigPath()
{
    static QString ret(appDataPath() + '\\' + YADAW::Base::ProductName + ".yml");
    return ret;
}
}