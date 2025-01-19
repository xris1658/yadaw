| [English](./BUILD.md) | 简体中文 |
| --------------------- | -------- |

# 构建 YADAW

可以在 Windows x64，Linux x64 和 macOS x64 上构建并使用 YADAW。如果未来有条件，
笔者会考虑添加 ARM 平台的支持。

## 在 Windows 上构建

### 使用 MSVC 构建

- 下载并安装 [Git for Windows](https://git-scm.com/download/win)，并将 git.exe 
  所在目录添加到系统环境变量 `Path` 中。
- 下载并安装 [Visual Studio 2022](https://visualstudio.microsoft.com/zh-hans/downloads/)。
  在 Visual Studio Installer 中安装时，勾选“工作负荷 -> 桌面应用和移动应用”中
  的“使用 C++ 的桌面开发”、“通用 Windows 平台开发”以及“语言包”中的
  “英语”。
- 下载 [Qt](https://www.qt.io/download-open-source) 6.5.1 或 6.6.1 或更高版本
  （[为什么？](https://bugreports.qt.io/browse/QTBUG-117852)）。
  在安装时，勾选“MSVC 2019 64-bit”和“Qt Debug Information Files”。安装完成后
  将 Qt 可执 行文件所在目录（<Qt 安装目录>\<版本号>\msvc2019_64\bin）添加到系统
  环境变量 `Path` 中。
- 下载 [vcpkg](https://github.com/microsoft/vcpkg)：
  ```shell
  git clone https://github.com/microsoft/vcpkg
  .\vcpkg\bootstrap-vcpkg.bat
  ```
  并将 vcpkg 目录添加到系统环境变量 `Path` 中。
- 安装一些必要的软件包：
  ```shell
  vcpkg install ade:x64-windows sqlite3:x64-windows sqlite-modern-cpp:x64-windows yaml-cpp:x64-windows cppwinrt
  ```
  - 虽然 Windows UWP SDK 自带 C++/WinRT SDK，但出于以下两个原因，构建项目使用
    `cppwinrt` 软件包：
    - YADAW 使用 C++20 标准，而一些旧版本的 Windows SDK（如 10.0.19041.0）随附的
      C++/WinRT SDK 仍使用 C++17 标准，以及实验性的协程支持。
    - 使用 AudioGraph API 时，要获取音频缓冲区，必须将 `IMemoryBufferReference`
      转换为 `IMemoryBufferByteAccess`，而此类不是 C++/WinRT SDK 中的一部分。新
      版的 C++/WinRT SDK 添加了 `IMemoryBufferReference::data()`，解决了这一问题。
- 下载 [VST3 SDK](https://github.com/steinbergmedia/vst3sdk) 的源码，并记住路径：
  ```shell
  cd <VST3 SDK 所在目录的路径>
  git clone --recursive https://github.com/steinbergmedia/vst3sdk
  ```
- 下载 [CLAP](https://github.com/free-audio/clap) 的源码，并记住路径：
  ```shell
  cd <CLAP 所在目录的路径>
  git clone https://github.com/free-audio/clap
  ```
- 下载 YADAW 的源码，配置并构建项目。启动 **Visual Studio 2022 开发者命令提示符**，
  并执行以下命令：
  ```shell
  git clone https://github.com/xris1658/yadaw
  cd ./yadaw
  mkdir build
  cd build
  cmake -S .. -B . \
    -DCMAKE_TOOLCHAIN_FILE=<vcpkg 所在目录的路径>/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVST3SDK_SOURCE_DIR=<VST3 SDK 所在目录的路径>/vst3sdk \
    -DCLAP_SOURCE_DIR=<CLAP 所在目录的路径>/clap
  cmake --build . --target YADAW -j 16
  ```
### 使用 MSYS2 构建

- 下载并安装 [MSYS2](https://www.msys2.org/)。  
  MSYS2 带有多套环境。下面的配置过程使用 MINGW64 环境，需要打开 MSYS2 MINGW64 后
  执行。

- 安装一些必要的软件包：
  ```shell
  pacman -S pactoys # pacboy
  pacman -S git
  pacboy -S gcc:x gdb:x cppwinrt:x cmake:x qt6-base:x qt6-declarative:x qt6-tools:x qt6-translations:x sqlite3:x yaml-cpp:x
  ```
- 下载 VST3 SDK 和 CLAP 的源码，并记住路径；
- 下载 [sqlite_modern_cpp](https://github.com/aminroosta/sqlite_modern_cpp) 的源
  码，并记住路径：
  ```shell
  cd <sqlite_modern_cpp 所在目录的路径>
  git clone https://github.com/aminroosta/sqlite_modern_cpp
  ```
- 下载 [ADE](https://github.com/opencv/ade)，构建并安装：
  ```shell
  git clone https://github.com/opencv/ade
  cd ./ade
  mkdir build
  cd build
  cmake -S .. -B . -DCMAKE_INSTALL_PREFIX=/mingw64
  cmake --build .
  cmake --install .
  ```
- 下载 YADAW 的源码，配置并构建项目：
  ```shell
  git clone https://github.com/xris1658/yadaw
  cd ./yadaw
  mkdir build
  cd build
  cmake -S .. -B . \
    -DVST3SDK_SOURCE_DIR=<VST3 SDK 所在目录的路径>/vst3sdk \
    -DCLAP_SOURCE_DIR=<CLAP 所在目录的路径>/clap \
    -DSQLITE_MODERN_CPP_INCLUDE_DIRS=<sqlite_modern_cpp 所在目录的路径>/sqlite_modern_cpp/hdr
  cmake --build . --target YADAW -j 16
  ```

## 在 Linux 上构建

以下内容适用于 Ubuntu 及其分支发行版。虽然笔者并未测试，但这些内容或许也适用于
Debian 及其分支发行版。

- 安装一些必要的软件包：
  ```shell
  sudo apt install git gcc g++ libasound2-dev libsqlite3-dev libxcb-icccm4-dev libyaml-cpp-dev
  # VST3 SDK 的依赖项
  sudo apt-get install libx11-xcb-dev libxcb-util-dev libxcb-cursor-dev \
  libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libfontconfig1-dev \
  libcairo2-dev libgtkmm-3.0-dev libsqlite3-dev libxcb-keysyms1-dev
  ```
- 下载并安装 CMake 3.25 或更高版本。此版本添加了 `LINUX` 字段，此项目用于检测
  目标操作系统。要使用旧版的 CMake 构建 YADAW，请调整
  `cmake_minimum_required` 中的版本，并将 `if(LINUX)` 替换为
  `if(UNIX AND NOT APPLE)`。
- 下载并安装 Qt（参见“使用 MSVC 构建”中描述的版本要求）；
- 下载 VST3 SDK、CLAP 和 sqlite_modern_cpp 的源码，并记住路径；
- 下载 ADE，构建并安装：
  ```shell
  git clone https://github.com/opencv/ade
  cd ./ade
  mkdir build
  cd build
  cmake -S .. -B .
  cmake --build .
  sudo cmake --install .
  ```
- 下载 YADAW 的源码，配置并构建项目：
  ```shell
  git clone https://github.com/xris1658/yadaw
  cd ./yadaw
  mkdir build
  cd build
  cmake -S .. -B . \
    -DCMAKE_PREFIX_PATH=<Qt 安装路径, e.g. /home/xris1658/apps/Qt/6.5.3/gcc_64> \
    -DVST3SDK_SOURCE_DIR=<VST3 SDK 所在目录的路径>/vst3sdk \
    -DCLAP_SOURCE_DIR=<CLAP 所在目录的路径>/clap \
    -DSQLITE_MODERN_CPP_INCLUDE_DIRS=<sqlite_modern_cpp 所在目录的路径>/sqlite_modern_cpp/hdr
  cmake --build . --target YADAW -j 16
  ```
  
### 输入法支持

Qt 6 中内置了 IBus 输入法支持。如果使用 Fcitx，则需要构建并安装 Fcitx 的平台集成
插件。步骤如下：
- 安装所需的依赖项：
  ```shell
  sudo apt install extra-cmake-modules
   ```
- 下载 fcitx5-qt 的源码，配置并构建项目：
  ```shell
  git clone https://github.com/fcitx/fcitx5-qt
  cd ./fcitx5-qt
  mkdir build
  cd build
  cmake -S .. -B . \
    -DCMAKE_PREFIX_PATH=<Qt 所在路径 e.g. /home/xris1658/apps/Qt/6.7.0/gcc_64> \
    -DENABLE_QT5=FALSE \
    -DBUILD_ONLY_PLUGIN=TRUE
  cmake --build . --target fcitx5platforminputcontextplugin-qt6
  ```

- 安装构建的插件：
  ```shell
  cp ./qt6/platforminputcontext/libfcitx5platforminputcontextplugin.so \
    <Qt 所在路径 e.g. /home/xris1658/apps/Qt/6.7.0/gcc_64>/plugins/platforminputcontexts
  ```
  
## 在 macOS 上构建

下面的步骤不需要安装 Xcode。此项目使用 C++20，因此无法使用版本较低的 macOS 工具
链构建。

- 安装 macOS 命令行开发者工具：
  ```shell
  xcode-select --install
  ```
  - 有趣小知识：如果未安装开发者工具，则在终端中运行开发者工具时，macOS 会询问用
  户，是否需要安装开发者工具。例如，只要在终端中运行 `gcc` 就能够安装工具了。
- 安装 [Homebrew](https://brew.sh/)。
- 安装 pkg-config：
  ```shell
  brew install pkg-config
  ```
- 下载并安装 CMake。可以从 cmake.org 上下载，也可以使用 Homebrew 安装：
  ```shell
  brew install cmake
  ```
  - 如果是从 cmake.org 上下载的，确保将可执行文件路径添加到环境变量 `PATH` 中：
    ```shell
    export PATH=$PATH:/Applications/CMake/Contents/bin
    ```
- 下载并安装 Ninja。如果是从 ninja-build.org 上下载的，确保将可执行文件路径添加
  到环境变量 `PATH` 中：
  ```shell
  export PATH=$PATH:<可执行文件路径, e.g. /Users/xris1658/apps/ninja-build>
  ```
- 下载并安装 Qt（参见“使用 MSVC 构建”中描述的版本要求）。安装完成后，将 Qt 可执
  行文件所在的目录（<Qt 安装目录>\<版本>\macos\bin）添加到环境变量 `PATH` 中。
- 下载 VST3 SDK，并记住路径。为配置项目，需要注释 cmake/modules/SMTG_DetectPlatform.cmake 的以下几行：
- ```cmake
  if(XCODE_VERSION VERSION_LESS "9")
      message(FATAL_ERROR "[SMTG] XCode 9 or newer is required")
  endif()
  ```
- 下载 CLAP，并记住路径。
- 下载并安装 vcpkg：
  ```shell
  git clone https://github.com/microsoft/vcpkg
  ./vcpkg/bootstrap-vcpkg.sh
  ```
- 用 vcpkg 安装一些依赖项：
  ```shell
  vcpkg install ade sqlite3 sqlite-modern-cpp yaml-cpp
  ```
  在用 vcpkg 安装程序包之前，需要安装 pkg-config，因此先前安装了 Homebrew。尽管
  Homebrew 是个在 macOS 上比较常用的包管理器，笔者还是选用了 vcpkg，因为其中有
  `ade` 库，OpenCV 中的一小部分。如果只使用 Homebrew，大概需要安装整个 OpenCV 库。
- 下载 YADAW 的源码，配置并构建项目：
  ```shell
  git clone https://github.com/xris1658/yadaw
  cd ./yadaw
  mkdir build
  cd build
  cmake -S .. -B . \
    -DCMAKE_TOOLCHAIN_FILE=<vcpkg 所在目录的路径>/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVST3SDK_SOURCE_DIR=<VST3 SDK 所在目录的路径>/vst3sdk \
    -DCLAP_SOURCE_DIR=<CLAP 所在目录的路径>/clap
  cmake --build . --target YADAW -j 16
  macqtdeploy ./YADAW.app -qmldir=./content
  cp -r ./content ./YADAW.app/Contents/Resources/qml
  cp -r ./Main ./YADAW.app/Contents/Resources/qml
  ```