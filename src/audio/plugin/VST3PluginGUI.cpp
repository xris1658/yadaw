#include "VST3PluginGUI.hpp"

#include "audio/util/VST3Helper.hpp"
#include "native/VST3Native.hpp"

namespace YADAW::Audio::Plugin
{
VST3PluginGUI::VST3PluginGUI(Steinberg::IPlugView* plugView):
    plugView_(plugView),
    window_(nullptr),
    frame_(this)
{
    plugView_->queryInterface(
        Steinberg::IPlugViewContentScaleSupport::iid,
        reinterpret_cast<void**>(&plugViewContentScaleSupport_)
    );
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
        releasePointer(plugViewContentScaleSupport_);
        plugView_->setFrame(nullptr);
        releasePointer(plugView_);
    }
}

bool VST3PluginGUI::attachToWindow(QWindow* window)
{
    if(window_)
    {
        return false;
    }
    window_ = window;
    auto devicePixelRatio = window->devicePixelRatio();
    if(plugViewContentScaleSupport_)
    {
        plugViewContentScaleSupport_->setContentScaleFactor(devicePixelRatio);
    }
    auto ret = plugView_->attached(
        reinterpret_cast<void*>(window->winId()),
        YADAW::Native::ViewType
    ) == Steinberg::kResultOk;
    if(ret)
    {
        if(!resizeViewCalled_)
        {
            Steinberg::ViewRect rect;
            plugView_->getSize(&rect);
            window_->resize(
                rect.getWidth() / devicePixelRatio,
                rect.getHeight() / devicePixelRatio
            );
        }
    }
    else
    {
        window_ = nullptr;
    }
    return ret;
}

QWindow* VST3PluginGUI::window()
{
    return window_;
}

const QWindow* VST3PluginGUI::window() const
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

bool VST3PluginGUI::resizableByUser() const
{
    if(plugView_)
    {
        return plugView_->canResize() == Steinberg::kResultTrue;
    }
    return false;
}

void VST3PluginGUI::connect()
{
    if(!isConnected_)
    {
        connections_[0] = QObject::connect(
            window_, &QWindow::widthChanged,
            [this](int)
            {
                onWindowSizeChanged();
            }
        );
        connections_[1] = QObject::connect(
            window_, &QWindow::heightChanged,
            [this](int)
            {
                onWindowSizeChanged();
            }
        );
        isConnected_ = true;
    }
}

void VST3PluginGUI::disconnect()
{
    if(isConnected_)
    {
        QObject::disconnect(connections_[0]);
        QObject::disconnect(connections_[1]);
        isConnected_ = false;
    }
}

bool VST3PluginGUI::isConnected() const
{
    return isConnected_;
}

void VST3PluginGUI::resizeViewCalled()
{
    resizeViewCalled_ = true;
}

void VST3PluginGUI::onWindowSizeChanged()
{
    auto devicePixelRatio = window_->devicePixelRatio();
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
        Steinberg::ViewRect* rects[2] = {&oldRect, &windowRect};
        auto* rect = rects[plugView_->onSize(&windowRect) == Steinberg::kResultOk];
        window_->resize(
            std::round(rect->getWidth() / devicePixelRatio),
            std::round(rect->getHeight() / devicePixelRatio)
        );
    }
}
}