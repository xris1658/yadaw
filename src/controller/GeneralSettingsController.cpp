#include "GeneralSettingsController.hpp"

#include "controller/ConfigController.hpp"

namespace YADAW::Controller
{
void GeneralSettingsController::setTranslation(const QString& name)
{
    auto config = YADAW::Controller::loadConfig();
    config["general"]["language"] = name.toStdString();
    saveConfig(config);
}
}