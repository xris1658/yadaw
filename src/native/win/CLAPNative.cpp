#include "native/CLAPNative.hpp"

#include <clap/entry.h>
#include <clap/ext/gui.h>

namespace YADAW::Native
{
using YADAW::Native::Library;
using YADAW::Audio::Plugin::CLAPPlugin;

const char* windowAPI = CLAP_WINDOW_API_WIN32;

void setWindow(clap_window& clapWindow, QWindow* window)
{
    clapWindow.win32 = reinterpret_cast<decltype(clapWindow.win32)>(window->winId());
}

YADAW::Audio::Plugin::CLAPPlugin createCLAPFromLibrary(Library& library)
{
    auto entry = library.getExport<const clap_plugin_entry*>("clap_entry");
    if(entry)
    {
        return CLAPPlugin(entry, library.path());
    }
    return {};
}

}