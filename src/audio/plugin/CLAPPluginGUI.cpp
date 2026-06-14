#include "CLAPPluginGUI.hpp"

#include "native/CLAPNative.hpp"

namespace YADAW::Audio::Plugin
{
CLAPPluginGUI::CLAPPluginGUI(const clap_plugin* plugin, const clap_plugin_gui* gui):
    plugin_(plugin),
    gui_(gui)
{
}

CLAPPluginGUI::~CLAPPluginGUI()
{
    CLAPPluginGUI::detachWithWindow();
    gui_ = nullptr;
    plugin_ = nullptr;
}

bool CLAPPluginGUI::usePhysicalPixelSize() const
{
#if defined(_WIN32) || defined(__linux__)
    return true;
#elif defined(__APPLE__)
    return false;
#endif
    return false;
}

bool CLAPPluginGUI::attachToWindow(QWindow* window)
{
    if((!window_) && gui_->create(plugin_, YADAW::Native::windowAPI, false))
    {
        window_ = window;
        fetchResizeHints();
#ifndef __APPLE__
        auto devicePixelRatio = window->devicePixelRatio();
        gui_->set_scale(plugin_, devicePixelRatio);
#endif
        std::uint32_t width;
        std::uint32_t height;

        if(auto resizeResult = gui_->get_size(plugin_, &width, &height))
        {
            window->resize(
#if __APPLE__
                width, height
#else
                width / devicePixelRatio, height / devicePixelRatio
#endif
            );
        }
        clapWindow_.api = YADAW::Native::windowAPI;
        YADAW::Native::setWindow(clapWindow_, window);
        if(gui_->set_parent(plugin_, &clapWindow_))
        {
            gui_->show(plugin_);
            return true;
        }
    }
    window_ = nullptr;
    return false;
}

const QWindow* CLAPPluginGUI::window() const
{
    return window_;
}

QWindow* CLAPPluginGUI::window()
{
    return window_;
}

bool CLAPPluginGUI::detachWithWindow()
{
    if(gui_ && window_)
    {
        gui_->destroy(plugin_);
        window_ = nullptr;
    }
    return true;
}

bool CLAPPluginGUI::resizableByUser() const
{
    if(gui_)
    {
        return gui_->can_resize(plugin_);
    }
    return false;
}

bool CLAPPluginGUI::adjustSize(QSize& size)
{
    if(gui_ && window_ && gui_->can_resize(plugin_))
    {
        auto width = static_cast<std::uint32_t>(size.width());
        auto height = static_cast<std::uint32_t>(size.height());
        if(gui_->adjust_size(plugin_, &width, &height))
        {
            size.setWidth(width);
            size.setHeight(height);
            return true;
        }
    }
    return false;
}

bool CLAPPluginGUI::resize(const QSize& size)
{
    return gui_ && gui_->set_size(plugin_, size.width(), size.height());
}

void CLAPPluginGUI::fetchResizeHints()
{
    gui_->get_resize_hints(plugin_, &resizeHints_);
}

clap_gui_resize_hints CLAPPluginGUI::resizeHints() const
{
    return resizeHints_;
}

const clap_plugin_gui* CLAPPluginGUI::gui()
{
    return gui_;
}
}