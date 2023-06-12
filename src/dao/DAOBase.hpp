#ifndef YADAW_SRC_DAO_DAOBASE
#define YADAW_SRC_DAO_DAOBASE

#include <QString>

namespace YADAW::DAO
{
const QString& appDataPath();

void createAppDataFolder();

const QString& appDatabaseFileName();

const QString& appDatabasePath();

const QString& appConfigFileName();

const QString& appConfigPath();
}

#endif // YADAW_SRC_DAO_DAOBASE
