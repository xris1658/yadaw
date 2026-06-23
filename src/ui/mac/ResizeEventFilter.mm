#if __APPLE__

#include "ui/ResizeEventFilter.hpp"

#include <QRect>
#include <QRectF>
#include <QSizeF>

#include <AppKit/NSWindow.h>

// Macros that control the debug messages, uncomment these on demand
// #define YADAW_DEBUG_RESIZE_EVENT_FILTER_MESSAGES 1
// #define YADAW_DEBUG_RESIZE_EVENT_FILTER_STATES   1

@interface WindowDelegate : NSObject <NSWindowDelegate>
{
    QWindow* window_;
    YADAW::UI::ResizeEventFilter* resizeEventFilter_;
    id<NSWindowDelegate> rollbackDelegate_;
}
+ (WindowDelegate*) create:(YADAW::UI::ResizeEventFilter&)eventFilter;
- (id) forwardingTargetForSelector:(SEL) aSelector;
@end

@implementation WindowDelegate

+ (WindowDelegate*) create:(YADAW::UI::ResizeEventFilter&)eventFilter;
{
    auto window = eventFilter.window();
    auto nsview = reinterpret_cast<NSView*>(window->winId());
    auto nswindow = [nsview window];
    auto rollbackDelegate = [nswindow delegate];
    auto ret = [[self alloc] init];
    ret->window_ = window;
    ret->resizeEventFilter_ = &eventFilter;
    ret->rollbackDelegate_ = rollbackDelegate;
    return ret;
}

- (id) forwardingTargetForSelector:(SEL) aSelector
{
    return rollbackDelegate_;
}

- (NSSize) windowWillResize:(NSWindow *) sender
                     toSize:(NSSize) frameSize
{
    QRectF rectf(QPointF(), QSizeF::fromCGSize(frameSize));
    std::fprintf(
        stderr,
        "[DEBUG] ResizeEventFilter: resizing to (%lf, %lf), %lf x %lf\n",
        rectf.left(), rectf.top(), rectf.width(), rectf.height()
    );
    auto rect = rectf.toRect();
    resizeEventFilter_->aboutToResize(YADAW::UI::ResizeEventFilter::DragPosition::Invalid, &rect);
    return rect.toRectF().size().toCGSize();
}

- (void) windowDidResize:(NSNotification *) notification
{
    auto geometry = window_->geometry();
    std::fprintf(
        stderr,
        "[DEBUG] ResizeEventFilter: resized to (%d, %d), %d x %d\n",
        geometry.left(), geometry.top(), geometry.width(), geometry.height()
    );
    resizeEventFilter_->resized(geometry);
}

- (void) windowWillStartLiveResize:(NSNotification *) notification
{
    std::fprintf(
        stderr,
        "[DEBUG] ResizeEventFilter: windowWillStartLiveResize\n"
    );
    resizeEventFilter_->startResize();
}

- (void) windowWillEndLiveResize:(NSNotification *) notification
{
    std::fprintf(
        stderr,
        "[DEBUG] ResizeEventFilter: windowWillEndLiveResize\n"
    );
    resizeEventFilter_->endResize();
}

@end

namespace YADAW::UI
{
ResizeEventFilter::ResizeEventFilter(QWindow& window):
    windowAndId_(window),
    delegate_([WindowDelegate create:*this])
{
    auto nsview = reinterpret_cast<NSView*>(windowAndId_.winId);
    auto nswindow = [nsview window];
    [nswindow setDelegate:reinterpret_cast<id<NSWindowDelegate>>(delegate_)];
}

ResizeEventFilter::~ResizeEventFilter()
{
    auto delegate = reinterpret_cast<id<NSWindowDelegate>>(delegate_);
    [delegate release];
}

ResizeEventFilter::FeatureSupportFlags ResizeEventFilter::getNativeSupportFlags()
{
    return FeatureSupportFlag::SupportsStartAndEndResize
         | FeatureSupportFlag::SupportsAboutToResize
         | FeatureSupportFlag::SupportsResized
         | FeatureSupportFlag::SupportsAdjustOnAboutToResize;
}
}

#endif
