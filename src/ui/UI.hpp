#ifndef YADAW_SRC_UI_UI
#define YADAW_SRC_UI_UI

#include <QQmlApplicationEngine>
#include <QQuickWindow>

namespace YADAW::UI
{
extern QQmlApplicationEngine* qmlApplicationEngine;
extern QQuickWindow* mainWindow;

void setSystemRender();

const QString& defaultFontDir();
}

#endif //YADAW_SRC_UI_UI
