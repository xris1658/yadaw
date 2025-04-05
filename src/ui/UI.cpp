#include "UI.hpp"
namespace YADAW::UI
{
const QString& defaultFontDir()
{
#if __APPLE__
    static QString ret = QCoreApplication::applicationDirPath() + "/../Resources/qml/content/fonts";
#else
    static QString ret =
        QCoreApplication::applicationDirPath()
        + YADAW::Native::PathSeparator + "content"
        + YADAW::Native::PathSeparator + "fonts";
#endif
    return ret;
}
}
