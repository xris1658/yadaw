#include "VestifalPluginGUI.hpp"

#include "audio/util/VestifalUtil.hpp"

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
        window_ = window;
        runDispatcher(effect_, EffectOpcode::effectOpenEditor, 0, 0, reinterpret_cast<void*>(window->winId()));
        VestifalRectangle* rectangle = nullptr;
        runDispatcher(effect_, EffectOpcode::effectGetRect, 0, 0, &rectangle);
        window_->setWidth(rectangle->right - rectangle->left);
        window_->setHeight(rectangle->bottom - rectangle->top);
        return true;
    }
}

QWindow* VestifalPluginGUI::window()
{
    return window_;
}

bool VestifalPluginGUI::detachWithWindow()
{
    runDispatcher(effect_, EffectOpcode::effectCloseEditor);
    return true;
}
}