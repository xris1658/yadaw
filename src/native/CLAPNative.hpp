#ifndef YADAW_SRC_NATIVE_CLAPNATIVE
#define YADAW_SRC_NATIVE_CLAPNATIVE

#include <clap/ext/gui.h>

#include <QWindow>

namespace YADAW::Native
{
extern const char* windowAPI;

void setWindow(clap_window& clapWindow, QWindow* window);
}

#endif // YADAW_SRC_NATIVE_CLAPNATIVE
