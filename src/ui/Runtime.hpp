#ifndef YADAW_SRC_UI_RUNTIME
#define YADAW_SRC_UI_RUNTIME

#include <QTimer>

class QQmlApplicationEngine;
class QQuickWindow;

namespace YADAW::UI
{
// Types are declared forwards so no errors occurs on building tests that do NOT
// use these pointers.
extern QQmlApplicationEngine* qmlApplicationEngine;
extern QQuickWindow* mainWindow;

QTimer& idleProcessTimer();
}
#endif // YADAW_SRC_UI_RUNTIME
