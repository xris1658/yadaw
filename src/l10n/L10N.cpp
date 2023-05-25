#include "L10N.hpp"

#include "native/Native.hpp"

#include <QCoreApplication>

namespace YADAW::L10N
{
const QString& l10nDirectoryPath()
{
    static QString ret = QCoreApplication::applicationDirPath() + YADAW::Native::PathSeparator + "l10n";
    return ret;
}
}