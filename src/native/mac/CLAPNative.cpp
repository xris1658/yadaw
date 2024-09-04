#if __APPLE__

#include "native/CLAPNative.hpp"

namespace YADAW::Native
{
const char* windowAPI = CLAP_WINDOW_API_COCOA;

void setWindow(clap_window& clapWindow, QWindow* window)
{
    auto id = window->winId();
    clapWindow.cocoa = reinterpret_cast<decltype(clapWindow.cocoa)*>(&id);
}
}

#endif