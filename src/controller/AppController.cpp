#include "AppController.hpp"

#include "base/Constants.hpp"
#include "dao/Config.hpp"
#include "dao/Database.hpp"
#include "native/Native.hpp"

#include <QFile>

namespace YADAW::Controller
{
const QString& appDataPath()
{
    static QString ret = YADAW::Native::roamingAppDataFolder() + "\\" + YADAW::Base::ProductName;
    return ret;
}

void initAppData()
{
    YADAW::DAO::initDatabase(YADAW::DAO::appDatabase());
}
}