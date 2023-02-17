#ifndef YADAW_SRC_NATIVE_WINDOWSDARKMODESUPPORT
#define YADAW_SRC_NATIVE_WINDOWSDARKMODESUPPORT

#include <QWindow>

#include <memory>

namespace YADAW::Native
{
class WindowsDarkModeSupport
{
    class Impl;
    friend std::unique_ptr<WindowsDarkModeSupport> std::make_unique<WindowsDarkModeSupport>();
private:
    WindowsDarkModeSupport();
public:
    WindowsDarkModeSupport(const WindowsDarkModeSupport&) = delete;
    WindowsDarkModeSupport(WindowsDarkModeSupport&&) = delete;
    ~WindowsDarkModeSupport();
public:
    static std::unique_ptr<WindowsDarkModeSupport>& instance();
public:
    void addWindow(QWindow* window);
    void removeWindow(QWindow* window);
private:
    std::unique_ptr<Impl> pImpl_;
};
}

#endif //YADAW_SRC_NATIVE_WINDOWSDARKMODESUPPORT
