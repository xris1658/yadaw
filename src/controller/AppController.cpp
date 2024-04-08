#include "AppController.hpp"

#include "controller/ConfigController.hpp"
#include "controller/DatabaseController.hpp"
#include "dao/Database.hpp"

namespace YADAW::Controller
{
void initApplication()
{
    auto config = loadConfig();
    //
    // TODO: Use config
    initializeApplicationDatabase();
    auto& database = YADAW::DAO::appDatabase();
    // TODO: Use database
}
}