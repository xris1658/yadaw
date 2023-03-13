#include "GeneralSettingsController.hpp"

#include "controller/ConfigController.hpp"

namespace YADAW::Controller
{
bool GeneralSettingsController::systemFontRendering()
{
    return YADAW::Controller::loadConfig()["general"]["system-font-rendering"].as<bool>();
}

void GeneralSettingsController::setSystemFontRendering(bool enabled)
{
    auto config = YADAW::Controller::loadConfig();
    config["general"]["system-font-rendering"] = enabled;
    saveConfig(config);
}
}