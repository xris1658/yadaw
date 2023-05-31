#include "UI.hpp"

#include "native/Native.hpp"

namespace YADAW::UI
{
QQmlApplicationEngine* qmlApplicationEngine = nullptr;
QQuickWindow* mainWindow = nullptr;

const QString& defaultFontDir()
{
    static QString ret =
        QCoreApplication::applicationDirPath()
        + YADAW::Native::PathSeparator + "content"
        + YADAW::Native::PathSeparator + "fonts";
    return ret;
}
}