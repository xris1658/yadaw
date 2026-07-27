#if _WIN32

#include "D3DFlipSwitcher.hpp"

#include <QtEnvironmentVariables>

#include <windows.h>
#include <winuser.h>

namespace YADAW::UI
{
D3DFlipSwitcher::D3DFlipSwitcher():
    neverFlip(qEnvironmentVariableIntegerValue("QT_D3D_NO_FLIP").value_or(0))
{}

void D3DFlipSwitcher::addWindow(QQuickWindow& window)
{
    if(auto rendererInterface = window.rendererInterface();
        rendererInterface->graphicsApi() == QSGRendererInterface::GraphicsApi::Direct3D11)
    {
        windows_[reinterpret_cast<HWND>(window.winId())] = WindowData {
            .window = &window
        };
    }
}

void D3DFlipSwitcher::removeWindow(QQuickWindow& window)
{
    windows_.erase(reinterpret_cast<HWND>(window.winId()));
}

bool D3DFlipSwitcher::nativeEventFilter(
    const QByteArray& eventType, void* message, qintptr* result)
{
    if((!neverFlip) && eventType == "windows_generic_MSG")
    {
        auto msg = reinterpret_cast<MSG*>(message);
        if(msg->message == WM_ENTERSIZEMOVE || msg->message == WM_EXITSIZEMOVE)
        {
            if(auto it = windows_.find(msg->hwnd); it != windows_.end())
            {
                it->second.fillRhiIfNeeded();
                if(it->second.swapChain)
                {
                    it->second.swapChain->destroy();
                    _putenv_s("QT_D3D_NO_FLIP", msg->message == WM_ENTERSIZEMOVE? "1": "0");
                    it->second.swapChain->createOrResize();
                }
            }
        }
    }
    return false;
}

void D3DFlipSwitcher::WindowData::fillRhiIfNeeded()
{
    if(auto rendererInterface = window->rendererInterface())
    {
        if(!swapChain)
        {
            swapChain = static_cast<QRhiSwapChain*>(
                rendererInterface->getResource(
                    window, QSGRendererInterface::Resource::RhiSwapchainResource
                )
            );
        }
        if(!rhi)
        {
            rhi = static_cast<QRhi*>(
                rendererInterface->getResource(
                    window, QSGRendererInterface::Resource::RhiResource
                )
            );
        }
    }
}
}

#endif
