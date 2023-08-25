#if __linux__

#include "native/CLAPNative.hpp"

namespace YADAW::Native
{

// X11 support only. Sorry about that.
const char* windowAPI = CLAP_WINDOW_API_X11;

void setWindow(clap_window& clapWindow, QWindow* window)
{
    auto id = window->winId();
    clapWindow.x11 = reinterpret_cast<decltype(clapWindow.x11)*>(&id)[0];
}
}

#endif