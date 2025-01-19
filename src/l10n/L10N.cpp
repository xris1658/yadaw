#include "L10N.hpp"

#include "native/Native.hpp"

#include <QCoreApplication>

namespace YADAW::L10N
{
const QString& l10nDirectoryPath()
{
#if __APPLE__
    static QString ret = QCoreApplication::applicationDirPath() + "/../Resources/l10n";
    qDebug() << ret;
#else
    static QString ret = QCoreApplication::applicationDirPath() + YADAW::Native::PathSeparator + "l10n";
#endif
    return ret;
}
}