#if __APPLE__

#import <AppKit/NSView.h>
#import <AppKit/NSWindow.h>

#include <QDebug>
#include <QWindow>

namespace YADAW::UI
{
void showNSWindowWithoutActivating(WId winId)
{
    auto nsview = reinterpret_cast<NSView*>(winId);
    auto window = [nsview window];
    [window setLevel:NSPopUpMenuWindowLevel];
    [window orderFrontRegardless];
    [window makeKeyWindow];
    auto windowController = [window windowController];
    [window hidesOnDeactivate];
    if(windowController)
    {
        [windowController showWindow:window];
    }
}

void toggleNSWindowFullscreen(WId winId)
{
    auto nsview = reinterpret_cast<NSView*>(winId);
    auto window = [nsview window];
    [window toggleFullScreen: nil];
}
}

#endif