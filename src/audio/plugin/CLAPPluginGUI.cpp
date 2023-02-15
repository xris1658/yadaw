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
    if(window_)
    {
        CLAPPluginGUI::detachWithWindow();
    }
    gui_ = nullptr;
    plugin_ = nullptr;
}

bool CLAPPluginGUI::attachToWindow(QWindow* window)
{
    if(window_)
    {
        gui_->create(plugin_, YADAW::Native::windowAPI, false);
        gui_->set_scale(plugin_, 1.0);
        std::uint32_t width;
        std::uint32_t height;
        auto resizeResult = gui_->get_size(plugin_, &width, &height);
        if(resizeResult)
        {
            window_->setWidth(width);
            window_->setHeight(height);
        }
        clapWindow_.api = YADAW::Native::windowAPI;
        YADAW::Native::setWindow(clapWindow_, window_);
        if(gui_->set_parent(plugin_, &clapWindow_))
        {
            gui_->show(plugin_);
            return true;
        }
    }
    return false;
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

const clap_plugin_gui* CLAPPluginGUI::gui()
{
    return gui_;
}
}