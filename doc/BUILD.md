| English | [简体中文](./BUILD-zh.md) |
| ------- | ------------------------- |

# Build YADAW

YADAW can be built and used on Windows x64 and Linux x64. If possible, I'd
consider add ARM and macOS support in the future.

## Build on Windows

### Build With MSVC

- Download and install [Git for Windows](https://git-scm.com/download/win), then
  add the path to git.exe to the system environment variable `Path`.
- Download and install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/).
  While installing with Visual Studio Installer, check "Desktop devleopment with
  C++" and "Universal Windows Platform development" in "Workloads -> Desktop &
  Mobile", and "English" in "Language packs".
  - Pay attention to your Windows SDK version installed to build the project.
    YADAW uses C++20 standard and C++/WinRT SDK, but the C++/WinRT SDK included
    in some old versions of Windows SDK (e.g. 10.0.19041.0) still uses C++17 and
    experimental coroutine support. I made some little changes to the relevant
    headers in Windows SDK 10.0.19041.0 to build this project(
    [xris1658/cppwinrt-19041-cpp20](https://github.com/xris1658/cppwinrt-19041-cpp20)).
    **You could edit those files manually, but please DO make backups!**
- Install [Qt 6.5](https://www.qt.io/download-open-source) or newer. While
  installing, check "MSVC 2019 64-bit" and "Qt Debug Information Files". After
  the installation, add the directory containing the Qt executable (<Qt install
  directory>\<version>\msvc2019_64\bin) to the system environment variable
  `Path`.
- Download [vcpkg](https://github.com/microsoft/vcpkg):
  ```shell
  git clone https://github.com/microsoft/vcpkg
  .\vcpkg\bootstrap-vcpkg.bat
  ```
  And add directory of vcpkg to the system environment variable `Path`.
- Install some prerequisites:
  ```shell
  vcpkg install ade:x64-windows sqlite3:x64-windows sqlite-modern-cpp:x64-windows yaml-cpp:x64-windows
  ```
- Download source of [VST3 SDK](https://github.com/steinbergmedia/vst3sdk),
  and remember the path to it:
  ```shell
  cd <path to directory of VST3 SDK>
  git clone --recursive https://github.com/steinbergmedia/vst3sdk
  ```
- Download source of [CLAP](https://github.com/free-audio/clap),
  and remember the path to it:
  ```shell
  cd <path to directory of CLAP>
  git clone https://github.com/free-audio/clap
  ```
- Download source of YADAW, configure and build the project. Launch **Developer
  Command Prompt for VS 2022**, and execute the following commands:
  ```shell
  git clone https://github.com/xris1658/yadaw
  cd ./yadaw
  mkdir build
  cd build
  cmake -S .. -B . \
    -DCMAKE_TOOLCHAIN_FILE=<path to vcpkg directory>/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVST3SDK_SOURCE_DIR=<path to directory of VST3 SDK>/vst3sdk \
    -DCLAP_SOURCE_DIR=<path to directory of CLAP>/clap \
  cmake --build . --target YADAW -j 16
  ```

### Build with MSYS2

- Download and install [MSYS2](https://www.msys2.org/).
  Thereare several environments available in MSYS2. The following process uses
  the MINGW64 environment, thus you should open MSYS2 MINGW64 to execute it.
- Install some prerequisites:
  ```shell
  pacman -S pactoys # pacboy
  pacman -S git
  pacboy -S gcc:x gdb:x cppwinrt:x cmake:x qt6-base:x qt6-declarative:x qt6-tools:x qt6-translations:x sqlite3:x yaml-cpp:x
  ```
- Download Windows SDK. MSYS2 is shipped with headers and libraries of Win32
  SDK, and the `cppwinrt` package contains headers of C++/WinRT SDK. However,
  source of YADAW uses contents in WinRT SDK (not to be confused with the
  C++/WinRT SDK) as well, so you still need to download Windows SDK manually,
  and install Windows SDK for UWP C++ Apps. (Actually only one header file with
  definition of an interface is used, but it's proprietary code after all, so I
  should not redistribute it without premission).
- Download source of VST3 SDK and CLAP, and remember the paths to them;
- Download source of [sqlite_modern_cpp](https://github.com/aminroosta/sqlite_modern_cpp),
  and remember the path to it:
  ```shell
  cd <path to directory of sqlite_modern_cpp>
  git clone https://github.com/aminroosta/sqlite_modern_cpp
  ```
- Download source of [ADE](https://github.com/opencv/ade), build and install it:
  ```shell
  git clone https://github.com/opencv/ade
  cd ./ade
  mkdir build
  cd build
  cmake -S .. -B . -DCMAKE_INSTALL_PREFIX=/mingw64
  cmake --build .
  cmake --install .
  ```
- Download source of YADAW, configure and build the project:
  ```shell
  git clone https://github.com/xris1658/yadaw
  cd ./yadaw
  mkdir build
  cd build
  cmake -S .. -B . \
    -DVST3SDK_SOURCE_DIR=<path to directory of VST3 SDK>/vst3sdk \
    -DCLAP_SOURCE_DIR=<path to directory of CLAP>/clap \
    -DSQLITE_MODERN_CPP_INCLUDE_DIRS=<path to directory of sqlite_modern_cpp>/sqlite_modern_cpp/hdr
  cmake --build . --target YADAW -j 16
  ```

### Known problems
- The application built with MSYS2 cannot handle paths with non-ASCII characters
  correctly. For instance, if the user folder name contains Chinese characters,
  then YADAW cannot read the configuration file of the application and crashes
  immediately. Such problem does not exist in the application built with MSVC.

## Build on Linux
The following steps are made for Ubuntu and its forks. Though not tested, those
might be usable on Debian and its forks.

- Install some prerequisites:
  ```shell
  sudo apt install git cmake gcc g++ libasound2-dev libsqlite3-dev libyaml-cpp-dev
  # Dependencies of VST3 SDK
  sudo apt-get install cmake libx11-xcb-dev libxcb-util-dev libxcb-cursor-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libfontconfig1-dev libcairo2-dev libgtkmm-3.0-dev libsqlite3-dev libxcb-keysyms1-dev
  ```
- Download and install Qt;
- Download source of VST3 SDK, CLAP and sqlite_modern_cpp, and remember the
  paths to them;
- Download source of [ADE](https://github.com/opencv/ade), build and install it:
  ```shell
  git clone https://github.com/opencv/ade
  cd ./ade
  mkdir build
  cd build
  cmake -S .. -B .
  cmake --build .
  cmake --install .
  ```
- Download source of YADAW, configure and build the project:
  ```shell
  git clone https://github.com/xris1658/yadaw
  cd ./yadaw
  mkdir build
  cd build
  cmake -S .. -B . \
    -DVST3SDK_SOURCE_DIR=<path to directory of VST3 SDK>/vst3sdk \
    -DCLAP_SOURCE_DIR=<path to directory of CLAP>/clap \
    -DSQLITE_MODERN_CPP_INCLUDE_DIRS=<path to directory of sqlite_modern_cpp>/sqlite_modern_cpp/hdr
  cmake --build . --target YADAW -j 16
  ```