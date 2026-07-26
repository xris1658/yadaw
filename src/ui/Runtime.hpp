#ifndef YADAW_SRC_UI_RUNTIME
#define YADAW_SRC_UI_RUNTIME

#include <QTimer>

#if _WIN32
#include "ui/win/D3DFlipSwitcher.hpp"
#endif

class QQmlApplicationEngine;
class QQuickWindow;

namespace YADAW::UI
{
// Types are declared forwards so no errors occurs on building tests that do NOT
// use these pointers.
extern QQmlApplicationEngine* qmlApplicationEngine;
extern QQuickWindow* mainWindow;

QTimer& idleProcessTimer();

#if _WIN32
YADAW::UI::D3DFlipSwitcher& d3dFlipSwitcher();
#endif
}
#endif // YADAW_SRC_UI_RUNTIME
