#if __APPLE__

#include "native/Window.hpp"

#import <AppKit/NSView.h>
#import <AppKit/NSWindow.h>

namespace YADAW::Native
{
void showWindowWithoutActivating(QWindow& window)
{
    window.setFlag(Qt::WindowType::WindowDoesNotAcceptFocus, true);
    window.showNormal();
    window.setVisible(true);
}

bool isWindowResizableByUser(QWindow& window)
{
    return true;
}

void setWindowResizableByUser(QWindow& window, bool resizable)
{
    // TODO
}

bool isWindowMaximized(QWindow& window)
{
    return window.visibility() == QWindow::Visibility::Maximized;
}

void toggleNSWindowFullscreen(WId winId)
{
    auto nsview = reinterpret_cast<NSView*>(winId);
    auto window = [nsview window];
    [window toggleFullScreen: nil];
}

void enterFullscreen(QWindow& window)
{
    if(window.visibility() == QWindow::Visibility::Maximized)
    {
        toggleNSWindowFullscreen(window.winId());
    }
    else
    {
        window.showFullScreen();
    }
}

void exitFullscreen(QWindow& window, bool previouslyMaximized)
{
    if(previouslyMaximized)
    {
        toggleNSWindowFullscreen(window.winId());
    }
    else
    {
        window.showNormal();
    }
}
}

#endif