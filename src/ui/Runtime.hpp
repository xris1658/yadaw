#ifndef YADAW_SRC_UI_RUNTIME
#define YADAW_SRC_UI_RUNTIME

#include <QQmlApplicationEngine>
#include <QQuickWindow>

namespace YADAW::UI
{
// Types are declared forwards so no errors occurs on building tests that do NOT
// use these pointers.
extern QQmlApplicationEngine* qmlApplicationEngine;
extern QQuickWindow* mainWindow;
}
#endif // YADAW_SRC_UI_RUNTIME
