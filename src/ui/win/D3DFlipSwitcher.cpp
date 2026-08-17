#ifdef YADAW_BUILD_MODIFIED_QRHID3D11

#include "D3DFlipSwitcher.hpp"

#include "qrhid3d11_p.h"

#include <QQuickGraphicsDevice>
#include <QQuickGraphicsConfiguration>

#include <windows.h>
#include <winuser.h>

namespace YADAW::UI
{
D3DFlipSwitcher::D3DFlipSwitcher(QObject* parent):
    QObject(parent),
    neverFlip(qEnvironmentVariableIntegerValue("QT_D3D_NO_FLIP").value_or(0))
{}

void D3DFlipSwitcher::addWindow(QQuickWindow& window, bool enableDebugLayer)
{
    if(auto rendererInterface = window.rendererInterface();
        rendererInterface->graphicsApi() == QSGRendererInterface::GraphicsApi::Direct3D11)
    {
        ModifiedRhi::QRhiD3D11InitParams params {
            .enableDebugLayer = enableDebugLayer
        };
        auto rhi = ModifiedRhi::QRhi::create(
            ModifiedRhi::QRhi::Implementation::D3D11,
            &params,
            ModifiedRhi::QRhi::Flags {}
        );
        window.setGraphicsDevice(
            QQuickGraphicsDevice::fromRhi(
                reinterpret_cast<QRhi*>(rhi)
            )
        );
        auto hwnd = reinterpret_cast<HWND>(window.winId());
        if(auto lowerBound = windows_.lower_bound(hwnd);
            lowerBound == windows_.end() || lowerBound->first != hwnd
        )
        {
            windows_.emplace_hint(lowerBound, hwnd, WindowData {
                .window = &window,
                .rhi = rhi,
            });
            QObject::connect(
                &window, &QQuickWindow::frameSwapped,
                this, &D3DFlipSwitcher::onWindowFrameSwapped
            );
        }
    }
}

void D3DFlipSwitcher::removeWindow(QQuickWindow& window)
{
    windows_.erase(reinterpret_cast<HWND>(window.winId()));
}

void D3DFlipSwitcher::onWindowFrameSwapped()
{
    auto window = qobject_cast<QQuickWindow*>(QObject::sender());
    auto hwnd = reinterpret_cast<HWND>(window->winId());
    if(auto it = windows_.find(hwnd); it != windows_.end())
    {
        if(auto pending = it->second.pendingRecreatingSwapChain();
            pending.load(std::memory_order::acquire))
        {
            it->second.swapChain->destroy();
            static_cast<ModifiedRhi::QD3D11SwapChain*>(
                it->second.swapChain
            )->toggleFlipMode();
            it->second.swapChain->createOrResize();
            pending.store(false, std::memory_order::release);
            pending.notify_one();
        }
    }
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
                    auto pending = it->second.pendingRecreatingSwapChain();
                    pending.wait(true, std::memory_order::acquire);
                    pending.store(true, std::memory_order::release);
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
            swapChain = static_cast<ModifiedRhi::QRhiSwapChain*>(
                rendererInterface->getResource(
                    window, QSGRendererInterface::Resource::RhiSwapchainResource
                )
            );
        }
    }
}

std::atomic_ref<bool> D3DFlipSwitcher::WindowData::pendingRecreatingSwapChain()
{
    return std::atomic_ref<bool>(pendingRecreatingSC);
}
}

#endif
