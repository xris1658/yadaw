#ifndef YADAW_SRC_UI_WINDOWANDCONNECTION
#define YADAW_SRC_UI_WINDOWANDCONNECTION

#include <QMetaObject>

class QWindow;

namespace YADAW::UI
{
struct WindowAndConnection
{
    QWindow* window;
    QMetaObject::Connection connection;
    WindowAndConnection(QWindow* window): window(window), connection() {}
    WindowAndConnection(const WindowAndConnection&) = default;
    WindowAndConnection(WindowAndConnection&&) noexcept = default;
    WindowAndConnection& operator=(const WindowAndConnection&) = default;
    WindowAndConnection& operator=(WindowAndConnection&&) = default;
    ~WindowAndConnection() noexcept = default;
};
}

#endif // YADAW_SRC_UI_WINDOWANDCONNECTION
