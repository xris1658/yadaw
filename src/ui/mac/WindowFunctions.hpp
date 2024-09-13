#ifndef YADAW_SRC_UI_MAC_WINDOWFUNCTIONS
#define YADAW_SRC_UI_MAC_WINDOWFUNCTIONS

#if __APPLE__

#include <QWindow>

namespace YADAW::UI
{
void toggleNSWindowFullscreen(WId winId);
}

#endif

#endif // YADAW_SRC_UI_MAC_WINDOWFUNCTIONS