#ifndef YADAW_SRC_AUDIO_PLUGIN_IPLUGINGUI
#define YADAW_SRC_AUDIO_PLUGIN_IPLUGINGUI

#include <QWindow>

namespace YADAW::Audio::Plugin
{
class IPluginGUI
{
public:
    virtual ~IPluginGUI() {}
public:
    virtual bool attachToWindow(QWindow* window) = 0;
    virtual QWindow* window() = 0;
    virtual bool detachWithWindow() = 0;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_IPLUGINGUI
