#ifndef YADAW_SRC_UI_WIN_D3DFLIPSWITCHER
#define YADAW_SRC_UI_WIN_D3DFLIPSWITCHER

#ifdef YADAW_BUILD_MODIFIED_QRHID3D11

#include <QAbstractNativeEventFilter>
#include <QFlags>
#include <QQuickWindow>

#include <qrhi.h>

#include <windows.h>
#include <winuser.h>

#include <atomic>
#include <map>

namespace YADAW::UI
{
// [*] Change flip mode on starting/ending resizing/moving `QQuickWindow`s that
//     runs with D3D11 backend.
//     This is the 1st of a 3-part workaround of `QQuickWindow` with D3D11 RHI
//     failing [the smooth resize test by Raph Levien](https://raphlinus.github.io/rust/gui/2019/06/21/smooth-resize-test.html).
//     This workaround is almost blatantly copied from [xi-editor/xi-win #21 also by Raph Levien](https://github.com/xi-editor/xi-win/pull/21).
//     See part 2 in tools/d3d11-rhi-changes and part 3 in src/main.cpp
//     TODO: Add functions to switch to/away from flip mode on demand since
//           windows can be resized/moved programmatically (like applying
//           `PropertyAnimation` on geometries of a `Window`)
class D3DFlipSwitcher: public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    D3DFlipSwitcher(QObject* parent = nullptr);
public:
    void addWindow(QQuickWindow& window, bool enableDebugLayer = false);
    void removeWindow(QQuickWindow& window);
public slots:
    void onWindowFrameSwapped();
public:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
private:
    bool neverFlip;
    struct WindowData
    {
        QQuickWindow* window;
        ModifiedRhi::QRhi* rhi = nullptr;
        ModifiedRhi::QRhiSwapChain* swapChain = nullptr;
        alignas(std::atomic_ref<bool>::required_alignment) bool pendingRecreatingSC = false;
        void fillRhiIfNeeded();
        std::atomic_ref<bool> pendingRecreatingSwapChain();
    };
    std::map<HWND, WindowData> windows_;
};
}

#endif

#endif // YADAW_SRC_UI_WIN_D3DFLIPSWITCHER
