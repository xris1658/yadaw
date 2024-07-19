#include "NativeEventFilter.hpp"

#include <QCoreApplication>

#if _WIN32
#include <windows.h>
#endif

NativeEventFilter::NativeEventFilter(QWindow& nativePopup):
    // Call QWindow::winId() outside `QAbstractNativeEventFilter::nativeEventFilter`;
    // otherwise infinite recursion will happen
    wid_(nativePopup.winId())
{
    QCoreApplication::instance()->installNativeEventFilter(this);
}

NativeEventFilter::~NativeEventFilter()
{
    QCoreApplication::instance()->removeNativeEventFilter(this);
}

bool NativeEventFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
#if _WIN32
    if(eventType == "windows_generic_MSG")
    {
        auto msg = static_cast<MSG*>(message);
        if(msg->hwnd == reinterpret_cast<HWND>(wid_))
        {
            if(msg->message == WM_MOUSEACTIVATE)
            {
                *result = MA_NOACTIVATE;
                return true;
            }
        }
    }
#endif
    return false;
}
