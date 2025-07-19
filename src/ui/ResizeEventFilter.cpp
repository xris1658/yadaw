#include "ResizeEventFilter.hpp"

#include <QCoreApplication>
#include <QMetaMethod>

#if _WIN32
#include <Windows.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#endif

namespace YADAW::UI
{
ResizeEventFilter::ResizeEventFilter(QWindow& window):
    windowAndId_(window)
{
    QCoreApplication::instance()->installNativeEventFilter(this);
}

ResizeEventFilter::~ResizeEventFilter()
{
    if(resizing_)
    {
        resized();
        endResize();
    }
    QCoreApplication::instance()->removeNativeEventFilter(this);
}

bool ResizeEventFilter::nativeEventFilter(
    const QByteArray& eventType, void* message, qintptr* result)
{
#if _WIN32
    constexpr DragPosition positions[] = {
        DragPosition::Left,
        DragPosition::Right,
        DragPosition::Top,
        DragPosition::TopLeft,
        DragPosition::TopRight,
        DragPosition::Bottom,
        DragPosition::BottomLeft,
        DragPosition::BottomRight
    };
    // TODO: Need to find out what the value actually is
    auto borderSize = 8;
    if(eventType == "windows_generic_MSG")
    {
        bool ret = false;
        static auto aboutToResizeSignal = QMetaMethod::fromSignal(
            &ResizeEventFilter::aboutToResize
        );
        auto msg = static_cast<MSG*>(message);
        if(msg->message == WM_NCLBUTTONDOWN
            && (msg->wParam >= HTLEFT && msg->wParam <= HTBOTTOMRIGHT))
        {
            position_ = positions[msg->wParam - HTLEFT];
            aboutToStartResize_ = true;
        }
        else if(msg->message == WM_SYSCOMMAND && msg->wParam == SC_SIZE)
        {
            // TODO: App Menu -> Size
        }
        else if(msg->message == WM_ENTERSIZEMOVE && aboutToStartResize_)
        {
            aboutToStartResize_ = false;
            resizing_ = true;
            startResize();
        }
        else if(msg->message == WM_WINDOWPOSCHANGING && resizing_ && !prevIsCaptureChanged_)
        {
            if(isSignalConnected(aboutToResizeSignal))
            {
                auto nativeRect = reinterpret_cast<WINDOWPOS*>(msg->lParam);
                QRect rect(nativeRect->x, nativeRect->y, nativeRect->cx, nativeRect->cy);
                RECT sysRect;
                GetWindowRect(reinterpret_cast<HWND>(windowAndId_.winId), &sysRect);
                auto margins = windowAndId_.window->frameMargins();
                margins.setLeft(margins.left() + borderSize);
                margins.setRight(margins.right() + borderSize);
                margins.setBottom(margins.bottom() + borderSize);
                rect = rect.marginsRemoved(margins);
                auto qtRect = windowAndId_.window->geometry();
                aboutToResize(position_, &rect);
                rect = rect.marginsAdded(margins);
                nativeRect->x = rect.left();
                nativeRect->y = rect.top();
                nativeRect->cx = rect.width();
                nativeRect->cy = rect.height();
                *result = 0;
                ret = true;
            }
        }
        else if(msg->message == WM_WINDOWPOSCHANGED && resizing_)
        {
            static auto resizedSignal = QMetaMethod::fromSignal(
                &ResizeEventFilter::resized
            );
            if(isSignalConnected(resizedSignal))
            {
                resized();
            }
        }
        else if(msg->message == WM_EXITSIZEMOVE && resizing_)
        {
            endResize();
            resizing_ = false;
        }
        else if(msg->message == WM_SIZING && resizing_) // Only if the resize is initiated
        {
            if(isSignalConnected(aboutToResizeSignal))
            {
                position_ = positions[msg->wParam - WMSZ_LEFT];
                auto nativeRect = reinterpret_cast<WINDOWPOS*>(msg->lParam);
                auto frameRect = windowAndId_.window->frameGeometry();
                QRect rect(nativeRect->x, nativeRect->y, nativeRect->cx, nativeRect->cy);
                auto margins = windowAndId_.window->frameMargins();
                margins.setLeft(borderSize);
                margins.setRight(borderSize);
                margins.setBottom(borderSize);
                rect = rect.marginsRemoved(margins);
                auto qtRect = windowAndId_.window->geometry();
                aboutToResize(position_, &rect);
                rect = rect.marginsAdded(margins);
                nativeRect->x = rect.left();
                nativeRect->y = rect.top();
                nativeRect->cx = rect.width();
                nativeRect->cy = rect.height();
                *result = 0;
                ret = true;
            }
        }
        prevIsCaptureChanged_ = msg->message == WM_CAPTURECHANGED;
        return ret;
    }
#endif
    return false;
}
}
