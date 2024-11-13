#include "PluginText.hpp"

#include <QCoreApplication>

namespace YADAW::Audio::Plugin
{
QString getDebugModeWarningTitle()
{
    return QCoreApplication::translate(
        "PluginText",
        "Warning: YADAW is in Debug Mode"
    );
}

QString getDebugModeWarningText()
{
    return QCoreApplication::translate(
        "PluginText",
        "<p>YADAW is being debugged. Some plugins with anti-piracy mechanism will refuse to work.</p>"
        "<p>Please try detaching this process with the debugger before loading plugins.</p>"
    );
}
}
