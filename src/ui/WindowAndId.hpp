#ifndef YADAW_SRC_UI_WINDOWANDID
#define YADAW_SRC_UI_WINDOWANDID

#include <QWindow>

namespace YADAW::UI
{
struct WindowAndId
{
    QWindow* window;
    WId winId;
    WindowAndId(QWindow& window): window(&window), winId(window.winId()) {}
    WindowAndId(const WindowAndId& rhs):
        window(rhs.window), winId(rhs.winId)
    {}
    ~WindowAndId() = default;
};
}

#endif // YADAW_SRC_UI_WINDOWANDID