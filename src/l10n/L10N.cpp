#include "L10N.hpp"

#include <QCoreApplication>

namespace YADAW::L10N
{
const QString& l10nDirectoryPath()
{
    static QString ret = QCoreApplication::applicationDirPath() + "\\l10n";
    return ret;
}
}