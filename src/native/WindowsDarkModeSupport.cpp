#include "WindowsDarkModeSupport.hpp"

#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#include "native/winrt/Forward.hpp"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.ViewManagement.Core.h>

#include <vector>

namespace YADAW::Native
{
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::UI::ViewManagement::Core;

class WindowsDarkModeSupport::Impl
{
public:
    Impl();
    ~Impl();
public:
    void addWindow(QWindow* window);
    void removeWindow(QWindow* window);
private:
    UISettings uiSettings_;
    std::vector<QWindow*> windows_;
    winrt::event_token eventToken_;
};

WindowsDarkModeSupport::Impl::Impl()
{
    eventToken_ = uiSettings_.ColorValuesChanged([this](const UISettings& sender, const IInspectable&)
    {
        auto color = sender.GetColorValue(UIColorType::Background);
        BOOL value = color.R * 77 + color.G * 150 + color.B * 29 < 32768;
        for(auto window: windows_)
        {
            DwmSetWindowAttribute(reinterpret_cast<HWND>(window->winId()), DWMWA_USE_IMMERSIVE_DARK_MODE,
                &value, sizeof(value));
        }
    });
}

WindowsDarkModeSupport::Impl::~Impl()
{
    uiSettings_.ColorValuesChanged(eventToken_);
}

void WindowsDarkModeSupport::Impl::addWindow(QWindow* window)
{
    if(auto it = std::find(windows_.begin(), windows_.end(), window);
        it == windows_.end())
    {
        windows_.emplace_back(window);
    }
    auto color = uiSettings_.GetColorValue(UIColorType::Background);
    BOOL value = color.R * 77 + color.G * 150 + color.B * 29 < 32768;
    DwmSetWindowAttribute(reinterpret_cast<HWND>(window->winId()), DWMWA_USE_IMMERSIVE_DARK_MODE,
        &value, sizeof(value));
}

void WindowsDarkModeSupport::Impl::removeWindow(QWindow* window)
{
    if(auto it = std::find(windows_.begin(), windows_.end(), window); it != windows_.end())
    {
        *it = windows_.back();
        windows_.pop_back();
    }
}

WindowsDarkModeSupport::WindowsDarkModeSupport(): pImpl_(std::make_unique<Impl>())
{
}

WindowsDarkModeSupport::~WindowsDarkModeSupport()
{
}

std::unique_ptr<WindowsDarkModeSupport>& WindowsDarkModeSupport::instance()
{
    static auto ret = std::make_unique<WindowsDarkModeSupport>();
    return ret;
}

void WindowsDarkModeSupport::addWindow(QWindow* window)
{
    pImpl_->addWindow(window);
}

void WindowsDarkModeSupport::removeWindow(QWindow* window)
{
    pImpl_->removeWindow(window);
}
}
