#include "VestifalPluginGUI.hpp"

#include "audio/util/VestifalHelper.hpp"

namespace YADAW::Audio::Plugin
{
VestifalPluginGUI::VestifalPluginGUI(AEffect& effect):
    effect_(&effect)
{
    //
}

VestifalPluginGUI::~VestifalPluginGUI()
{
    if(window_)
    {
        VestifalPluginGUI::detachWithWindow();
    }
}

bool VestifalPluginGUI::attachToWindow(QWindow* window)
{
    if(!window_)
    {
        auto attachResult = runDispatcher(effect_, EffectOpcode::effectOpenEditor, 0, 0, reinterpret_cast<void*>(window->winId()));
        if(attachResult)
        {
            window_ = window;
            VestifalRectangle* rectangle = nullptr;
            runDispatcher(effect_, EffectOpcode::effectGetRect, 0, 0, &rectangle);
            window_->setWidth(rectangle->right - rectangle->left);
            window_->setHeight(rectangle->bottom - rectangle->top);
        }
        return attachResult;
    }
    return false;
}

QWindow* VestifalPluginGUI::window()
{
    return window_;
}

const QWindow* VestifalPluginGUI::window() const
{
    return window_;
}

bool VestifalPluginGUI::detachWithWindow()
{
    if(window_)
    {
        runDispatcher(effect_, EffectOpcode::effectCloseEditor);
        window_ = nullptr;
        return true;
    }
    return false;
}
}