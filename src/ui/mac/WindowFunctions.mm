#if __APPLE__

#import <AppKit/NSView.h>
#import <AppKit/NSWindow.h>

#include <QDebug>
#include <QWindow>

namespace YADAW::UI
{
void toggleNSWindowFullscreen(WId winId)
{
    auto nsview = reinterpret_cast<NSView*>(winId);
    auto window = [nsview window];
    [window toggleFullScreen: nil];
}
}

#endif