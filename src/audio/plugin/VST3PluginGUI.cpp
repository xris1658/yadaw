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
    auto attachResult = plugView_->attached(reinterpret_cast<void*>(window->winId()),
        YADAW::Native::ViewType);
    if(attachResult == Steinberg::kResultOk)
    {
        auto devicePixelRatio = window->devicePixelRatio();
        if(plugViewContentScaleSupport_)
        {
            plugViewContentScaleSupport_->setContentScaleFactor(devicePixelRatio);
        }
        window_ = window;
        Steinberg::ViewRect rect;
        if(plugView_->getSize(&rect) == Steinberg::kResultOk)
        {
            // The initial information might be incorrect. In this case,
            // we give it another try
            if(rect.getWidth() <= 0 || rect.getHeight() <= 0)
            {
                window_->resize(400, 400);
                onWindowSizeChanged();
            }
            else
            {
                window_->resize(std::round(rect.getWidth() / devicePixelRatio), std::round(rect.getHeight() / devicePixelRatio));
            }
            connect();
        }
    }
    return attachResult == Steinberg::kResultOk;
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

void VST3PluginGUI::connect()
{
    connections_[0] = QObject::connect(window_, &QWindow::widthChanged,
        [this](int)
        {
            onWindowSizeChanged();
        }
    );
    connections_[1] = QObject::connect(window_, &QWindow::heightChanged,
        [this](int)
        {
            onWindowSizeChanged();
        }
    );
}

void VST3PluginGUI::disconnect()
{
    QObject::disconnect(connections_[0]);
    QObject::disconnect(connections_[1]);
}

void VST3PluginGUI::onWindowSizeChanged()
{
    if(!inCallback_)
    {
        auto devicePixelRatio = window_->devicePixelRatio();
        inCallback_ = true;
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
            window_->resize(std::round(rect->getWidth() / devicePixelRatio), std::round(rect->getHeight() / devicePixelRatio));
        }
        inCallback_ = false;
    }
}
}