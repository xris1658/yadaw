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
- Install [Qt](https://www.qt.io/download-open-source) 6.5.1, 6.6.1 or newer
  versions ([why not Qt 6.5.2 or 6.5.3?](https://bugreports.qt.io/browse/QTBUG-117852)).
  While installing, check "MSVC 2019 64-bit" and "Qt Debug Information Files".
  After the installation, add the directory containing the Qt executable (<Qt
  install directory>\<version>\msvc2019_64\bin) to the system environment
  variable `Path`.
- Download [vcpkg](https://github.com/microsoft/vcpkg):
  ```shell
  git clone https://github.com/microsoft/vcpkg
  .\vcpkg\bootstrap-vcpkg.bat
  ```
  And add directory of vcpkg to the system environment variable `Path`.
- Install some prerequisites:
  ```shell
  vcpkg install ade:x64-windows sqlite3:x64-windows sqlite-modern-cpp:x64-windows yaml-cpp:x64-windows cppwinrt
  ```
  - Though C++/WinRT SDK is included in the Windows UWP SDK, we use the
    `cppwinrt` package to build the project for two reasons:
    - YADAW uses C++20, but the C++/WinRT SDK included in some old versions of
      Windows SDK (e.g. 10.0.19041.0) is not updated, and still uses C++17 and
      experimental coroutine support.
    - To get the audio buffer while using the AudioGraph API, we have to convert
      `IMemoryBufferReference` to `IMemoryBufferByteAccess`, which is not part
      of the C++/WinRT SDK. `IMemoryBufferReference::data()` is added in the
      newer versions of C++/WinRT SDK, which fixes this problem.
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

## Build on Linux
The following steps are made for Ubuntu and its forks. Though not tested, those
might be usable on Debian and its forks.

- Install some prerequisites:
  ```shell
  sudo apt install git gcc g++ libasound2-dev libsqlite3-dev libxcb-icccm4-dev libyaml-cpp-dev
  # Dependencies of VST3 SDK
  sudo apt-get install libx11-xcb-dev libxcb-util-dev libxcb-cursor-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libfontconfig1-dev libcairo2-dev libgtkmm-3.0-dev libsqlite3-dev libxcb-keysyms1-dev
  ```
- Download and install CMake 3.25 or higher. The field `LINUX` which is used to
  check target OS is added in this version. To build YADAW with older CMake,
  set version of `cmake_minimum_required` and replace `if(LINUX)` with
  `if(UNIX AND NOT APPLE)`.
- Download and install Qt (see the version requirement described in
  Build With MSVC);
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

### IME Support

Qt 6 has a built-in IBus IME support. If you're using Fcitx, then you need to build and  
install the platform integration plugin of Fcitx. The steps are as follows:
- Install prerequisites needed:
  ```shell
  sudo apt install extra-cmake-modules
   ```
- Download source of fcitx5-qt, configure and build the project:
  ```shell
  git clone https://github.com/fcitx/fcitx5-qt
  cd ./fcitx5-qt
  mkdir build
  cd build
  cmake -S .. -B . \
    -DCMAKE_PREFIX_PATH=<Path of Qt e.g. /home/xris1658/apps/Qt/6.7.0/gcc_64> \
    -DENABLE_QT5=FALSE \
    -DBUILD_ONLY_PLUGIN=TRUE
  cmake --build . --target fcitx5platforminputcontextplugin-qt6
  ```

- Install the built plugin:
  ```shell
  cp ./qt6/platforminputcontext/libfcitx5platforminputcontextplugin.so \
    <Path of Qt e.g. /home/xris1658/apps/Qt/6.7.0/gcc_64>/plugins/platforminputcontexts
  ```
