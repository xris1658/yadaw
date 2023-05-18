#include "UI.hpp"

#include "native/Native.hpp"

namespace YADAW::UI
{
QQmlApplicationEngine* qmlApplicationEngine = nullptr;
QQuickWindow* mainWindow = nullptr;

void setSystemRender()
{
    // Check if the application is being debugged. If yes, then system rendering is disabled.
    // On Windows, MacType running in service mode doesn't work in debug mode, and system rendering
    // makes text pixelated.
    // See https://github.com/snowie2000/mactype/wiki/Enable-registry-mode-manually
    // on how to make MacType run in registry mode.
#if(WIN32)
    if(!YADAW::Native::isDebuggerPresent())
    {
#endif
        QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
#if(WIN32)
    }
#endif
}
}