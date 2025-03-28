#ifndef YADAW_SRC_NATIVE_NATIVE
#define YADAW_SRC_NATIVE_NATIVE

#include "util/Endian.hpp"

#include <QString>
#include <QtGlobal>

#if _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
using HResult = HRESULT;
#endif

#include <array>
#include <chrono>
#include <thread>

namespace YADAW::Native
{

#if _WIN32
constexpr auto CLSIDStringLength = 38;
constexpr char PathSeparator = '\\';

using WindowType = HWND;
using ErrorCodeType = decltype(GetLastError());
#elif __APPLE__
constexpr char PathSeparator = '/';
using ErrorCodeType = int;
#elif __linux__
constexpr char PathSeparator = '/';

using ErrorCodeType = int;
#endif

// Windows: <User folder>\AppData\Roaming
// Linux: $HOME/.local
const QString& appDataFolder();

// Usually <System Driver>\Program Files
const QString& programFilesFolder();

void openSpecialCharacterInput();

constexpr YADAW::Util::Endian SystemEndian =
    Q_BYTE_ORDER == Q_LITTLE_ENDIAN?
        YADAW::Util::Endian::LittleEndian:
        YADAW::Util::Endian::BigEndian;

constexpr YADAW::Util::Endian ReverseSystemEndian =
    Q_BYTE_ORDER == Q_LITTLE_ENDIAN?
        YADAW::Util::Endian::BigEndian:
        YADAW::Util::Endian::LittleEndian;

bool isDebuggerPresent();

QString errorMessageFromErrorCode(ErrorCodeType errorCode);

QString getProductVersion(const QString& path);

void sleepFor(std::chrono::steady_clock::duration duration);

const std::vector<QString>& defaultPluginDirectoryList();

bool setBadMemoryAccessHandler();

#if _WIN32
struct WindowsVersion
{
    DWORD majorVersion;
    DWORD minorVersion;
    DWORD buildVersion;
};

WindowsVersion getWindowsVersion();
#endif
}

#endif //  YADAW_SRC_NATIVE_NATIVE
