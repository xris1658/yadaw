#ifndef YADAW_SRC_NATIVE_SHELL
#define YADAW_SRC_NATIVE_SHELL

#include <QString>

namespace YADAW::Native
{
QString getFileBrowserName();

void locateFileInExplorer(const QString& path);

#if __linux__
inline const char* getDesktop()
{
    auto ret = std::getenv("XDG_CURRENT_DESKTOP");
    if(!ret)
    {
        ret = std::getenv("XDG_SESSION_DESKTOP");
    }
    return ret;
}
#endif
}

#endif // YADAW_SRC_NATIVE_SHELL