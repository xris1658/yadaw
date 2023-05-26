#include "GeneralSettingsController.hpp"

#include "controller/ConfigController.hpp"

namespace YADAW::Controller
{
bool GeneralSettingsController::systemFontRendering()
{
    return YADAW::Controller::loadConfig()["general"]["system-font-rendering"].as<bool>();
}

bool GeneralSettingsController::systemFontRenderingWhileDebugging()
{
    return YADAW::Controller::loadConfig()["general"]["system-font-rendering-while-debugging"].as<bool>();
}

void GeneralSettingsController::setSystemFontRendering(bool enabled)
{
    auto config = YADAW::Controller::loadConfig();
    config["general"]["system-font-rendering"] = enabled;
    saveConfig(config);
}

void GeneralSettingsController::setSystemFontRenderingWhileDebugging(bool enabled)
{
    auto config = YADAW::Controller::loadConfig();
    config["general"]["system-font-rendering-while-debugging"] = enabled;
    saveConfig(config);
}

void GeneralSettingsController::setTranslation(const QString& name)
{
    auto config = YADAW::Controller::loadConfig();
    config["general"]["language"] = name.toStdString();
    saveConfig(config);
}
}