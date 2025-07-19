#include "ResizeEventFilter.hpp"

#include <QCoreApplication>
#include <QMetaMethod>

#if _WIN32
#include <Windows.h>
#endif

namespace YADAW::UI
{
ResizeEventFilter::ResizeEventFilter(QWindow& window):
    windowAndId_(window)
{
    QCoreApplication::instance()->installEventFilter(this);
}

ResizeEventFilter::~ResizeEventFilter()
{
    if(resizing_)
    {
        resized();
        endResize();
    }
    QCoreApplication::instance()->removeEventFilter(this);
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
        auto msg = static_cast<MSG*>(message);
        if(msg->message == WM_SYSCOMMAND && msg->wParam == SC_SIZE)
        {
            aboutToStartResize_ = true;
        }
        else if(msg->message == WM_ENTERSIZEMOVE && aboutToStartResize_)
        {
            aboutToStartResize_ = false;
            resizing_ = true;
            startResize();
        }
        else if(msg->message == WM_SIZING)
        {
            static auto aboutToResizeSignal = QMetaMethod::fromSignal(
                &ResizeEventFilter::aboutToResize
            );
            if(isSignalConnected(aboutToResizeSignal))
            {
                auto nativeRect = reinterpret_cast<RECT*>(msg->lParam);
                QRect rect(
                    nativeRect->left,
                    nativeRect->top,
                    nativeRect->right - nativeRect->left,
                    nativeRect->bottom - nativeRect->top
                );
                aboutToResize(positions[msg->wParam - 1], &rect);
                nativeRect->left = rect.left();
                nativeRect->top = rect.top();
                nativeRect->right = rect.right();
                nativeRect->bottom = rect.bottom();
                *result = TRUE;
                return true;
            }
        }
        else if(msg->message == WM_SIZE)
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
    }
#endif
    return false;
}
}
