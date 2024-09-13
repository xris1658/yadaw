#ifndef YADAW_SRC_UI_UI
#define YADAW_SRC_UI_UI

#include <QString>
#include <QWindow>

namespace YADAW::UI
{
const QString& defaultFontDir();

void showWindowWithoutActivating(WId winId);

// Enters fullscreen. If the window was maximized, this function tries setting
// window state from maximized to fullscreen without showing the intermediate
// normal visibility.
// Since it relies on private `QPlatformWindow` implementations, this function
// might not work correctly on several Qt versions.
void enterFullscreen(QWindow& window);

// Exits fullscreen. If the window was maximized before full screen, this
// function tries setting window state from fullscreen to maximized without
// showing the intermediate normal visibility.
// Since it relies on private `QPlatformWindow` implementations, this function
// might not work correctly on several Qt versions.
void exitFullscreen(QWindow& window, bool previouslyMaximized);

bool isWindowResizable(QWindow& window);

void setWindowResizable(QWindow& window, bool resizable);
}

#endif // YADAW_SRC_UI_UI
