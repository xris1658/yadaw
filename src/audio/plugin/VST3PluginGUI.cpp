#include "VST3PluginGUI.hpp"

#include "native/VST3Native.hpp"

namespace YADAW::Audio::Plugin
{
VST3PluginGUI::VST3PluginGUI(Steinberg::IPlugView* plugView):
    plugView_(plugView),
    window_(nullptr),
    frame_(this)
{
    plugView_->setFrame(&frame_);
}

VST3PluginGUI::~VST3PluginGUI()
{
    if(window_)
    {
        VST3PluginGUI::detachWithWindow();
    }
    if(plugView_)
    {
        plugView_->release();
        plugView_ = nullptr;
    }
}

bool VST3PluginGUI::attachToWindow(QWindow* window)
{
    if(window_)
    {
        return false;
    }
    auto attachResult = plugView_->attached(reinterpret_cast<void*>(window->winId()),
        YADAW::Native::ViewType);
    if(attachResult == Steinberg::kResultOk)
    {
        window_ = window;
        Steinberg::ViewRect rect;
        if(plugView_->getSize(&rect) == Steinberg::kResultOk)
        {
            window_->setWidth(rect.getWidth());
            window_->setHeight(rect.getHeight());
            connect();
        }
    }
    return attachResult == Steinberg::kResultOk;
}

QWindow* VST3PluginGUI::window()
{
    return window_;
}

bool VST3PluginGUI::detachWithWindow()
{
    if(!window_)
    {
        return true;
    }
    disconnect();
    auto detachResult = plugView_->removed();
    if(detachResult == Steinberg::kResultOk)
    {
        window_ = nullptr;
    }
    return detachResult == Steinberg::kResultOk;
}

void VST3PluginGUI::connect()
{
    connections_[0] = QObject::connect(window_, &QWindow::widthChanged,
        [this](int) { onWindowSizeChanged(); });
    connections_[1] = QObject::connect(window_, &QWindow::heightChanged,
        [this](int) { onWindowSizeChanged(); });
}

void VST3PluginGUI::disconnect()
{
    QObject::disconnect(connections_[0]);
    QObject::disconnect(connections_[1]);
}

void VST3PluginGUI::onWindowSizeChanged()
{
    // https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical+Documentation/Workflow+Diagrams/Resize+View+Call+Sequence.html#initiated-from-host
    Steinberg::ViewRect oldRect;
    plugView_->getSize(&oldRect);
    Steinberg::ViewRect windowRect;
    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = window_->width();
    windowRect.bottom = window_->height();
    if(plugView_->checkSizeConstraint(&windowRect) == Steinberg::kResultOk)
    {
        disconnect();
        window_->setWidth(windowRect.getWidth());
        window_->setHeight(windowRect.getHeight());
        if(plugView_->onSize(&windowRect) != Steinberg::kResultOk)
        {
            window_->setWidth(oldRect.getWidth());
            window_->setHeight(oldRect.getHeight());
        }
        connect();
    }
}
}