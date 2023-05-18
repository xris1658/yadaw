#include "native/CLAPNative.hpp"

#include <clap/entry.h>
#include <clap/ext/gui.h>

namespace YADAW::Native
{
using YADAW::Native::Library;
using YADAW::Audio::Plugin::CLAPPlugin;

// X11 support only. Sorry about that.
const char* windowAPI = CLAP_WINDOW_API_X11;

void setWindow(clap_window& clapWindow, QWindow* window)
{
    auto id = window->winId();
    auto handle = window->handle();
    clapWindow.x11 = reinterpret_cast<decltype(clapWindow.x11)*>(&id)[0];
}
}