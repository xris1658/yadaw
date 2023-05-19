#if(WIN32)

#include "native/CLAPNative.hpp"

namespace YADAW::Native
{
const char* windowAPI = CLAP_WINDOW_API_WIN32;

void setWindow(clap_window& clapWindow, QWindow* window)
{
    clapWindow.win32 = reinterpret_cast<decltype(clapWindow.win32)>(window->winId());
}
}

#endif