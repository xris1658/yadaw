#ifndef YADAW_NATIVE_NATIVE
#define YADAW_NATIVE_NATIVE

#include "util/Endian.hpp"

#include <QList>
#include <QString>
#include <QtGlobal>

#if(WIN32)
#include <Windows.h>
#endif

#include <array>
#include <chrono>

namespace YADAW::Native
{

#if(WIN32)
constexpr auto CLSIDStringLength = 38;

using WindowType = HWND;
using ErrorCodeType = decltype(GetLastError());
#elif(__linux__)
using ErrorCodeType = int;
#endif

// Usually <User folder>\AppData\Roaming
const QString& roamingAppDataFolder();

// Usually <System Driver>\Program Files
const QString& programFilesFolder();

// Usually <User folder>\AppData\Local
const QString& localAppDataFolder();

void openSpecialCharacterInput();

void showFileInExplorer(const QString& path);

int getProcessCPUCoreCount();

std::int64_t currentTimeValueInNanosecond();

std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> currentTimePointInNanosecond();

void setThreadPriorityToTimeCritical();

constexpr YADAW::Util::Endian SystemEndian =
    Q_BYTE_ORDER == Q_LITTLE_ENDIAN?
        YADAW::Util::Endian::LittleEndian:
        YADAW::Util::Endian::BigEndian;

constexpr YADAW::Util::Endian ReverseSystemEndian =
    Q_BYTE_ORDER == Q_LITTLE_ENDIAN?
        YADAW::Util::Endian::BigEndian:
        YADAW::Util::Endian::LittleEndian;

// Check if the application is being debugged. Currently only used to decide whether system
// rendering should be enabled.
bool isDebuggerPresent();

QString errorMessageFromErrorCode(ErrorCodeType errorCode);

QString getProductVersion(const QString& path);

void sleepFor(std::chrono::steady_clock::duration duration);

const std::vector<QString>& defaultPluginDirectoryList();

void locateFileInExplorer(const QString& path);
}

#endif // YADAW_NATIVE_NATIVE
