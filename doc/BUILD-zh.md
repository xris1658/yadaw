| [English](./BUILD.md) | 简体中文 |
| --------------------- | -------- |

# 构建 YADAW

可以在 Windows x64 和 Linux x64 上构建并使用 YADAW。如果未来有条件，笔者会考虑
添加 ARM 平台以及 macOS 的支持。

## 在 Windows 上构建

### 使用 MSVC

- 下载并安装 [Git for Windows](https://git-scm.com/download/win)，并将 git.exe 
  所在目录添加到系统环境变量 `Path` 中。
- 下载并安装 [Visual Studio 2022](https://visualstudio.microsoft.com/zh-hans/downloads/)。
  在 Visual Studio Installer 中安装时，勾选“工作负荷 -> 桌面应用和移动应用”中
  的“使用 C++ 的桌面开发”、“通用 Windows 平台开发”以及“语言包”中的
  “英语”。
  - 请留意安装并用于构建项目的 Windows SDK 版本。YADAW 项目本身使用 C++20 标准，
    且使用了 C++/WinRT SDK，而一些旧版本的 Windows SDK（如 10.0.19041.0）随附的
    C++/WinRT SDK 仍使用 C++17 标准，以及实验性的协程支持。笔者参考新版本的头文
    件对 Windows SDK 10.0.19041.0 中的相关头文件稍作更改，用于构建此项目（
    [xris1658/cppwinrt-19041-cpp20](https://github.com/xris1658/cppwinrt-19041-cpp20)）。
    **读者可以自行修改对应的文件，但务必记得备份！**
- 下载 [Qt 6.5](https://www.qt.io/download-open-source) 或更高版本。在安装时，勾
  选“MSVC 2019 64-bit”和“Qt Debug Information Files”。安装完成后，将 Qt 可执
  行文件所在目录（<Qt 安装目录>\<版本号>\msvc2019_64\bin）添加到系统环境变量
  `Path` 中。
- 下载 [vcpkg](https://github.com/microsoft/vcpkg)：
  ```shell
  git clone https://github.com/microsoft/vcpkg
  .\vcpkg\bootstrap-vcpkg.bat
  ```
  并将 vcpkg 目录添加到系统环境变量 `Path` 中。
- 安装一些必要的软件包：
  ```shell
  vcpkg install ade:x64-windows sqlite3:x64-windows sqlite-modern-cpp:x64-windows yaml-cpp:x64-windows
  ```
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
    -DCLAP_SOURCE_DIR=<CLAP 所在目录的路径>/clap \
  cmake --build . --target YADAW -j 16
  ```
### 使用 MSYS2

- 下载并安装 [MSYS2](https://www.msys2.org/)。  
  MSYS2 带有多套环境。下面的配置过程使用 MINGW64 环境，需要打开 MSYS2 MINGW64 后
  执行。

- 安装一些必要的软件包：
  ```shell
  pacman -S pactoys # pacboy
  pacman -S git
  pacboy -S gcc:x gdb:x cppwinrt:x cmake:x qt6-base:x qt6-declarative:x qt6-tools:x qt6-translations:x sqlite3:x yaml-cpp:x
  ```
- 下载 Windows SDK。MSYS2 中自带 Win32 SDK 的头文件和库，`cppwinrt` 软件包中包含
  了 C++/WinRT SDK 的头文件。然而，由于 YADAW 的源码还使用了 WinRT SDK 中的内容
  ，因 此除了上面安装的 `cppwinrt` 软件包之外，用户仍需手动下载 Windows SDK，并
  安装 Windows SDK for UWP C++ Apps。（虽然只用了一个写有接口定义的头文件，但毕
  竟属于专有代码，笔者似乎不应擅自分发。）
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

### 一些已知问题
- 使用 MSYS2 构建的应用程序似乎无法正确处理带有非 ASCII 字符的路径。例如，当用户
  文件夹名带中文字符时，YADAW 无法读取应用程序的配置文件，并立即崩溃。使用 MSVC
  构建的程序似乎没有这一问题。

## 在 Linux 上构建

以下内容适用于 Ubuntu 及其分支发行版。虽然笔者并未测试，但这些内容或许也适用于
Debian 及其分支发行版。

- 安装一些必要的软件包：
  ```shell
  sudo apt install git cmake gcc g++ libasound2-dev libsqlite3-dev libyaml-cpp-dev
  # VST3 SDK 的依赖项
  sudo apt-get install cmake libx11-xcb-dev libxcb-util-dev libxcb-cursor-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libfontconfig1-dev libcairo2-dev libgtkmm-3.0-dev libsqlite3-dev libxcb-keysyms1-dev
  ```
- 下载并安装 Qt；
- 下载 VST3 SDK、CLAP 和 sqlite_modern_cpp 的源码，并记住路径；
- 下载 ADE，构建并安装：
  ```shell
  git clone https://github.com/opencv/ade
  cd ./ade
  mkdir build
  cd build
  cmake -S .. -B .
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