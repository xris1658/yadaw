# namespace `ModifiedRhi`

Add modified `QRhiD3D11`. All files in this directory are copied from Qt sources of the version I'm building with. Since Qt RHI maintains compatibility within a minor release,
I have to copy multiple versions of the source.


This is the 2nd of a 3-part workaround of `QQuickWindow` with D3D11 RHI failing
[the smooth resize test by Raph Levien](https://raphlinus.github.io/rust/gui/2019/06/21/smooth-resize-test.html).

This workaround is almost blatantly copied from [xi-editor/xi-win #21 also by Raph Levien](https://github.com/xi-editor/xi-win/pull/21).

See part 1 in src/ui/win/D3DFlipSwitcher.hpp and part 3 in src/main.cpp

## Steps

TODO: Make patches for certain versions of Qt source so that developers and
maintainers can use the modified RHI by simply applying these. The following
steps are just a guide for changing the source files in case patches for newer
versions of Qt don't exist.

### Fix compilation and link errors
- Replace `QT_BEGIN_NAMESPACE` with `namespace ModifiedRhi {`
- Replace `QT_END_NAMESPACE` with `}`
- Remove `#include "qrhi*.h"` other than `#include "qrhid3d11_p.h"` in `qrhi.cpp`; then
- Remove `new` statements other than `QRhiD3D11` in `QRhiImplementation::newInstance` (`QRhi::create` in older versions)
- Replace `#include <rhi/qrhi.h>` (if any) with `#include "qrhi.h"`
- Replace `#include <rhi/qrhi_platform.h>` (if any) with `#include "qrhi_platform.h"`
- Replace `#include "qshader.h"` with `#include <rhi/qshader.h>`
- Replace `#include "vs_test_p.h"` with `#include <private/vs_test_p.h>`
- Move `class QWindow;` in `qrhi.h` and `qrhi_platform.h` out of `namespace ModifiedRhi`
- Replace `Q_DECLARE_TYPEINFO\((.+)` with `}\nQ_DECLARE_TYPEINFO(ModifiedRhi::$1\nnamespace ModifiedRhi {`
- Move `Q_LOGGING_CATEGORY`, `Q_LOGGING_CATEGORY_WITH_ENV_OVERRIDE` in `qrhi.cpp` out of `ModifiedRhi`
- Remove
  ```cpp
  constexpr int QRhi::MAX_MIP_LEVELS;
  constexpr int QRhiShaderResourceBinding::LAYOUT_DESC_ENTRIES_PER_BINDING;
  ```
  if you downloaded the code from MSYS2 (See [msys2/MINGW-packages @ 435cc33](https://github.com/msys2/MINGW-packages/commit/435cc33edc7475231f35f535a82a36302c58a1ee))
- Replace `class Q_GUI_EXPORT` with `class`
- Replace `struct Q_GUI_EXPORT` with `struct`
- Replace
  ```cpp
  Q_DECLARE_LOGGING_CATEGORY(QRHI_LOG_INFO)
  Q_DECLARE_LOGGING_CATEGORY(QRHI_LOG_RUB)
  ```
  in `qrhi_p.h` with
  ```cpp
  }
  inline namespace QtPrivateLogging
  {
  Q_DECLARE_LOGGING_CATEGORY(QRHI_LOG_INFO)
  }
  Q_DECLARE_LOGGING_CATEGORY(QRHI_LOG_RUB)
  namespace ModifiedRhi
  {
  ```

### Fix warnings
- Replace `friend Q_GUI_EXPORT` in `qrhi.h` with `friend`
- Replace `^Q_GUI_EXPORT ` in `qrhi.h` with empty

### Add functions

### Add flip functions
- Add member functions in `QD3D11SwapChain`:
  ```cpp
  bool QD3D11SwapChain::useFlipMode() const
  {
      QRHI_RES_RHI(QRhiD3D11);
      return !rhiD->useLegacySwapchainModel;
  }
  
  void QD3D11SwapChain::setFlipMode(bool flipMode)
  {
      QRHI_RES_RHI(QRhiD3D11);
      rhiD->useLegacySwapchainModel = !flipMode;
  }
  
  void QD3D11SwapChain::toggleFlipMode()
  {
      QRHI_RES_RHI(QRhiD3D11);
      rhiD->useLegacySwapchainModel = !rhiD->useLegacySwapchainModel;
  }
  ```
- Move code in `QRhiD3D11::create`:
  ```cpp
  if (!rhiD->useLegacySwapchainModel) {
      if (qEnvironmentVariableIsSet("QT_D3D_MAX_FRAME_LATENCY"))
          maxFrameLatency = UINT(qMax(0, qEnvironmentVariableIntValue("QT_D3D_MAX_FRAME_LATENCY")));
  } else {
      maxFrameLatency = 0;
  }
  ```
  to `QD3D11SwapChain::createOrResize` below
  ```cpp
  QRHI_RES_RHI(QRhiD3D11);
  ```
  and replace `maxFrameLatency` with `rhiD->maxFrameLatency`, replace `useLegacySwapchainModel` with `rhiD->useLegacySwapchainModel`.