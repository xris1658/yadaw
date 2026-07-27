#ifndef YADAW_SRC_UI_WIN_D3DFLIPSWITCHER
#define YADAW_SRC_UI_WIN_D3DFLIPSWITCHER

#if _WIN32

#include <QAbstractNativeEventFilter>
#include <QFlags>
#include <QQuickWindow>

#include <rhi/qrhi.h>

#include <windows.h>
#include <winuser.h>

#include <map>

namespace YADAW::UI
{
// [*] Change flip mode on starting/ending resizing/moving `QQuickWindow`s that
//     runs with D3D11 backend.
//     This is the 1st of a 3-part workaround of `QQuickWindow` with D3D11 RHI
//     failing [the smooth resize test by Raph Levien](https://raphlinus.github.io/rust/gui/2019/06/21/smooth-resize-test.html).
//     This workaround is almost blatantly copied from [xi-editor/xi-win #21 also by Raph Levien](https://github.com/xi-editor/xi-win/pull/21).
//     See part 2 in tools/d3d11-rhi-changes and part 3 in src/main.cpp
class D3DFlipSwitcher: public QAbstractNativeEventFilter
{
public:
    D3DFlipSwitcher();
public:
    void addWindow(QQuickWindow& window);
    void removeWindow(QQuickWindow& window);
public:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    bool neverFlip;
    struct WindowData
    {
        QQuickWindow* window;
        QRhi* rhi = nullptr;
        QRhiSwapChain* swapChain = nullptr;
        void fillRhiIfNeeded();
    };
    std::map<HWND, WindowData> windows_;
};
}

#endif

#endif // YADAW_SRC_UI_WIN_D3DFLIPSWITCHER
