#include "ResizeEventFilter.hpp"

#include <QCoreApplication>
#include <QMetaMethod>

#if _WIN32
#include <Windows.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#endif

#if _WIN32

#include <mutex>

QMargins clientMargins;

std::once_flag getMarginsFlag;

void getMargins()
{
    auto sizeFrame = GetSystemMetrics(SM_CXSIZEFRAME);
    auto fixedFrame = GetSystemMetrics(SM_CXFIXEDFRAME);
    auto border = GetSystemMetrics(SM_CXBORDER);
    auto titleBar = GetSystemMetrics(SM_CYCAPTION);
    clientMargins.setLeft(
        GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXBORDER)
    );
    clientMargins.setRight(clientMargins.left());
    clientMargins.setBottom(
        GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYBORDER)
    );
    clientMargins.setTop(clientMargins.bottom() + GetSystemMetrics(SM_CYCAPTION));
}
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
        resized(windowAndId_.window->geometry());
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
    if(eventType == "windows_generic_MSG")
    {
        bool ret = false;
        static auto aboutToResizeSignal = QMetaMethod::fromSignal(
            &ResizeEventFilter::aboutToResize
        );
        auto msg = static_cast<MSG*>(message);
        if(msg->hwnd != reinterpret_cast<HWND>(windowAndId_.winId))
        {
            ret = false;
        }
        else if(msg->message == WM_NCLBUTTONDOWN
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
                std::call_once(getMarginsFlag, &getMargins);
                auto nativeRect = reinterpret_cast<WINDOWPOS*>(msg->lParam);
                QRect rect(nativeRect->x, nativeRect->y, nativeRect->cx, nativeRect->cy);
                rect = rect.marginsRemoved(clientMargins);
                aboutToResize(position_, &rect);
                rect = rect.marginsAdded(clientMargins);
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
                auto nativeRect = reinterpret_cast<WINDOWPOS*>(msg->lParam);
                QRect rect(nativeRect->x, nativeRect->y, nativeRect->cx, nativeRect->cy);
                rect = rect.marginsRemoved(clientMargins);
                resized(rect);
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
                std::call_once(getMarginsFlag, &getMargins);
                position_ = positions[msg->wParam - WMSZ_LEFT];
                auto nativeRect = reinterpret_cast<RECT*>(msg->lParam);
                QRect rect(nativeRect->left, nativeRect->top,
                    nativeRect->right - nativeRect->left, nativeRect->bottom - nativeRect->top
                );
                rect = rect.marginsRemoved(clientMargins);
                aboutToResize(position_, &rect);
                rect = rect.marginsAdded(clientMargins);
                nativeRect->left = rect.left();
                nativeRect->top = rect.top();
                nativeRect->right = rect.width();
                nativeRect->bottom = rect.height();
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
