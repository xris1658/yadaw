#ifndef YADAW_SRC_NATIVE_SHELL
#define YADAW_SRC_NATIVE_SHELL

#include <QString>

namespace YADAW::Native
{
QString getFileBrowserName();

void locateFileInExplorer(const QString& path);
}

#endif // YADAW_SRC_NATIVE_SHELL