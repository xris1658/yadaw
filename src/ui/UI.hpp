#ifndef YADAW_SRC_UI_UI
#define YADAW_SRC_UI_UI

#include <QQmlApplicationEngine>
#include <QQuickWindow>

namespace YADAW::UI
{
extern QQmlApplicationEngine* qmlApplicationEngine;
extern QQuickWindow* mainWindow;

const QString& defaultFontDir();

// Set window state to full screen.
// If fullscreen is set to false, then the window state is set by maximized.
void setWindowFullScreen(QWindow& window, bool fullscreen, bool maximized);
}

#endif // YADAW_SRC_UI_UI
