#if __linux__

#include "native/Shell.hpp"

#include <QProcess>
#include <QStringList>

#include <cstdlib>
#include <cstring>

namespace YADAW::Native
{
void locateFileInExplorer(const QString& path)
{
    auto desktop = std::getenv("XDG_SESSION_DESKTOP");
    QString program; program.reserve(std::size("nautilus"));
    QStringList args;
    if(std::strstr(desktop, "KDE"))
    {
        program = "dolphin";
        args << path << "--new-window" << "--select";
    }
    else if(std::strstr(desktop, "GNOME"))
    {
        program = "nautilus";
        args << "-s" << path;
    }
    // If the path is a directory, then open it (which is not what we want);
    // If the path is a file, then locate it.
    else if(std::strstr(desktop, "XFCE"))
    {
        program = "thunar";
        args << path;
    }
    // TODO: Add COSMIC Files (https://github.com/pop-os/cosmic-files) once a
    //       stable version release comes out, as well as other DEs
    if(!args.empty())
    {
        QProcess::startDetached(program, args);
    }
    // GNOME:      nautilus -s [URI]
    // Xfce:       thunar [URI]
    // KDE Plasma: dolphin [URI] --new-window --select
    // LXDE:       Not supported by pcmanfm?
    // Cinnamon:   nemo [URI]
}
}

#endif