#ifndef YADAW_SRC_UI_UI
#define YADAW_SRC_UI_UI

#include <QQmlApplicationEngine>
#include <QQuickWindow>

namespace YADAW::UI
{
extern QQmlApplicationEngine* qmlApplicationEngine;
extern QQuickWindow* mainWindow;

const QString& defaultFontDir();

// Try setting window state from maximized to fullscreen showing the
// intermediate normal visibility.
// Since it relies on private `QPlatformWindow` implementations, this function
// might not work correctly on several Qt versions.
void setMaximizedWindowToFullScreen(QWindow& window);

// Try setting window state from fullscreen to maximized showing the
// intermediate normal visibility.
// Since it relies on private `QPlatformWindow` implementations, this function
// might not work correctly on several Qt versions.
void setFullScreenWindowToMaximized(QWindow& window);
}

#endif // YADAW_SRC_UI_UI
