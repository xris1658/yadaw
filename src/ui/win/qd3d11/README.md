Add modified `QRhiD3D11`.

This is the 2nd of a 3-part workaround of `QQuickWindow` with D3D11 RHI failing
[the smooth resize test by Raph Levien](https://raphlinus.github.io/rust/gui/2019/06/21/smooth-resize-test.html).

This workaround is almost blatantly copied from [xi-editor/xi-win #21 also by Raph Levien](https://github.com/xi-editor/xi-win/pull/21).

See part 1 in src/ui/win/D3DFlipSwitcher.hpp and part 3 in src/main.cpp