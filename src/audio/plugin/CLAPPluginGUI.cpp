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

bool CLAPPluginGUI::attachToWindow(QWindow* window)
{
    if((!window_) && gui_->create(plugin_, YADAW::Native::windowAPI, false))
    {
        window_ = window;
        fetchResizeHints();
        auto devicePixelRatio = window->devicePixelRatio();
        gui_->set_scale(plugin_, devicePixelRatio);
        std::uint32_t width;
        std::uint32_t height;
        auto resizeResult = gui_->get_size(plugin_, &width, &height);
        if(resizeResult)
        {
            window->resize(
                std::round(width / devicePixelRatio),
                std::round(height / devicePixelRatio)
            );
        }
        clapWindow_.api = YADAW::Native::windowAPI;
        YADAW::Native::setWindow(clapWindow_, window);
        if(gui_->set_parent(plugin_, &clapWindow_))
        {
            gui_->show(plugin_);
            if(gui_->can_resize(plugin_))
            {
                connect();
            }
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
        if(gui_->can_resize(plugin_))
        {
            disconnect();
        }
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

void CLAPPluginGUI::requestResizeCalled()
{
    requestResizeCalled_ = true;
}

void CLAPPluginGUI::fetchResizeHints()
{
    gui_->get_resize_hints(plugin_, &resizeHints_);
}

clap_gui_resize_hints CLAPPluginGUI::resizeHints() const
{
    return resizeHints_;
}

void CLAPPluginGUI::connect()
{
    connections_[0] = QObject::connect(window_, &QWindow::widthChanged,
        [this](int) { onWindowSizeChanged(); });
    connections_[1] = QObject::connect(window_, &QWindow::heightChanged,
        [this](int) { onWindowSizeChanged(); });
}

void CLAPPluginGUI::disconnect()
{
    QObject::disconnect(connections_[0]);
    QObject::disconnect(connections_[1]);
}

const clap_plugin_gui* CLAPPluginGUI::gui()
{
    return gui_;
}

void CLAPPluginGUI::onWindowSizeChanged()
{
    if(requestResizeCalled_)
    {
        requestResizeCalled_ = false;
        return;
    }
    auto devicePixelRatio = window_->devicePixelRatio();
    std::uint32_t oldWidth, oldHeight;
    gui_->get_size(plugin_, &oldWidth, &oldHeight);
    std::uint32_t width = window_->width() * devicePixelRatio;
    std::uint32_t height = window_->height() * devicePixelRatio;
    if(gui_->adjust_size(plugin_, &width, &height))
    {
        disconnect();
        if(!gui_->set_size(plugin_, width, height))
        {
            gui_->get_size(plugin_, &width, &height);
            window_->resize(
                std::round(width / devicePixelRatio),
                std::round(height / devicePixelRatio)
            );
        }
        connect();
        return;
    }
    window_->resize(
        std::round(width / devicePixelRatio),
        std::round(height / devicePixelRatio)
    );
}
}