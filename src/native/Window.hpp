#ifndef YADAW_SRC_NATIVE_WINDOW
#define YADAW_SRC_NATIVE_WINDOW

#include <QWindow>

namespace YADAW::Native
{
// Q: Why does this function exist when there is a flag called
//    `Qt::WindowType::WindowDoesNotAcceptFocus`?
// A: On X11 with only flags set, if a native popup is shown by clicking
//    outside the mouse (e.g. clicking a menu bar item that shows a menu in its
//    native popup), then clicking inside the popup generates a `WM_TAKE_FOCUS`
//    event, which closes the popup and leaves the click event not handled.
void showWindowWithoutActivating(QWindow& window);

bool isWindowResizable(QWindow& window);

void setWindowResizable(QWindow& window, bool resizable);

// Enters fullscreen. If the window was maximized, this function tries setting
// window state from maximized to fullscreen without showing the intermediate
// normal visibility.
// Since it relies on private `QPlatformWindow` implementations, this function
// might not work correctly on several Qt versions.
// TODO: Test on multiple platforms (specifically, Windows 10/11) with window
//       animations enabled, and see if the behavior is right.
void enterFullscreen(QWindow& window);

// Exits fullscreen. If the window was maximized before full screen, this
// function tries setting window state from fullscreen to maximized without
// showing the intermediate normal visibility.
// Since it relies on private `QPlatformWindow` implementations, this function
// might not work correctly on several Qt versions.
// TODO: Test on multiple platforms (specifically, Windows 10/11) with window
//       animations enabled, and see if the behavior is right.
void exitFullscreen(QWindow& window, bool previouslyMaximized);
}

#endif // YADAW_SRC_NATIVE_WINDOW