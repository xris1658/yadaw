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
    neverFlip(
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        qEnvironmentVariableIntegerValue("QT_D3D_NO_FLIP").value_or(0)
#else
        qEnvironmentVariableIntValue("QT_D3D_NO_FLIP")
#endif
    )
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
        if(auto sc = static_cast<ModifiedRhi::QD3D11SwapChain*>(
            it->second.swapChain
        ))
        {
            auto usingFlip = it->second.usingFlipMode();
            if(auto flip = usingFlip.load(std::memory_order_acquire);
                static_cast<bool>(flip) != sc->useFlipMode())
            {
                sc->destroy();
                sc->setFlipMode(flip);
                sc->createOrResize();
            }
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
                    auto usingFlip = it->second.usingFlipMode();
                    usingFlip.fetch_xor(1U, std::memory_order_acq_rel);
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

std::atomic_ref<std::uint_fast8_t> D3DFlipSwitcher::WindowData::usingFlipMode()
{
    return std::atomic_ref<std::uint_fast8_t>(usingFlip);
}
}

#endif
