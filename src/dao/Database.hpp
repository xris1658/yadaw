#ifndef YADAW_SRC_DAO_DATABASE
#define YADAW_SRC_DAO_DATABASE

#include <sqlite_modern_cpp.h>

namespace YADAW::DAO
{
sqlite::database& appDatabase();

void initDatabase(sqlite::database& database);
}

#endif // YADAW_SRC_DAO_DATABASE
