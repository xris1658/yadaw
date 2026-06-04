#ifndef YADAW_SRC_AUDIO_PLUGIN_PLUGINWINDOW
#define YADAW_SRC_AUDIO_PLUGIN_PLUGINWINDOW

#include "audio/plugin/IPluginGUI.hpp"
#include "ui/ResizeEventFilter.hpp"

#include <QWindow>

namespace YADAW::Audio::Plugin
{
class PluginWindow: public QWindow
{
public:
    PluginWindow();
    ~PluginWindow();
public:
    QWindow& pluginFrame();
    QWindow* topBar();
    void setTopBar(QWindow* bar);
public:
    void setGUI(YADAW::Audio::Plugin::IPluginGUI& pluginGUI);
    void resetGUI();
public:
    void resizeFromPlugin(const QSize& size);
    bool canClose() const;
    void setCanClose(bool arg);
protected:
    void closeEvent(QCloseEvent* closeEvent) override;
private slots:
    void onAboutToResize(
        YADAW::UI::ResizeEventFilter::DragPosition dragPosition,
        QRect* rect
    );
    void onResized(QRect rect);
    void onTopBarHeightChanged(int height);
    void onVisibleChanged(bool visible);
private:
    QWindow pluginFrame_;
    YADAW::UI::ResizeEventFilter resizeEventFilter_;
    QWindow* topBar_ = nullptr;
    YADAW::Audio::Plugin::IPluginGUI* pluginGUI_ = nullptr;
    enum ResizeOp: std::uint8_t
    {
        Repositioning = 1 << 0,
        ResizingFromPlugin = 1 << 1
    };
    std::underlying_type_t<ResizeOp> resizeOps_;
    bool canClose_ = true;
};
}

#endif // YADAW_SRC_AUDIO_PLUGIN_PLUGINWINDOW
