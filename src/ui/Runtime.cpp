#include "ui/Runtime.hpp"

#include <mutex>

namespace YADAW::UI
{
QQmlApplicationEngine* qmlApplicationEngine = nullptr;
QQuickWindow* mainWindow = nullptr;

std::once_flag initializeIdleProcessTimerFlag;

QTimer& idleProcessTimer()
{
    static QTimer timer;
    std::call_once(
        initializeIdleProcessTimerFlag,
        []()
        {
            timer.setInterval(16);
            timer.setSingleShot(false);
            timer.start();
        }
    );
    return timer;
}

#if _WIN32
YADAW::UI::D3DFlipSwitcher& d3dFlipSwitcher()
{
    static YADAW::UI::D3DFlipSwitcher ret;
    return ret;
}
#endif
}
