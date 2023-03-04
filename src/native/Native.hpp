#ifndef YADAW_NATIVE_NATIVE
#define YADAW_NATIVE_NATIVE

#include "util/Endian.hpp"

#include <QList>
#include <QString>
#include <QtGlobal>

#include <Windows.h>

#include <array>
#include <chrono>

namespace YADAW::Native
{
constexpr auto CLSIDStringLength = 38;

using SystemTimeType = SYSTEMTIME;

using ThreadMaskType = std::uint64_t;

using SystemTimeStringType = std::array<char, 18>;

using WindowType = HWND;

#if(WIN32)
using ErrorCodeType = decltype(GetLastError());
#endif

// Usually <User folder>\AppData\Roaming
const QString& roamingAppDataFolder();

// Usually <System Driver>\Program Files
const QString& programFilesFolder();

// Usually <User folder>\AppData\Local
const QString& localAppDataFolder();

SystemTimeType getCurrentTime();

SystemTimeType getLaunchTime();

// Format systen time to YYYYMMDDHHMMSS
SystemTimeStringType formatTime(const SystemTimeType& time);

void openSpecialCharacterInput();

void showFileInExplorer(const QString& path);

int getProcessCPUCoreCount();

ThreadMaskType getMIDIClockThreadAffinity();

ThreadMaskType setThreadMask(ThreadMaskType mask);

std::int64_t currentTimeValueInNanosecond();

std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> currentTimePointInNanosecond();

void setThreadPriorityToTimeCritical();

inline constexpr YADAW::Util::Endian endian()
{
    return Q_BYTE_ORDER == Q_LITTLE_ENDIAN? YADAW::Util::Endian::LittleEndian:
        Q_BYTE_ORDER == Q_BIG_ENDIAN? YADAW::Util::Endian::BigEndian:
            YADAW::Util::Endian::UnknownEndian;
}

// Check if the application is being debugged. Currently only used to decide whether system
// rendering should be enabled.
bool isDebuggerPresent();

QString errorMessageFromErrorCode(ErrorCodeType errorCode);

QString getProductVersion(const QString& path);

void sleepFor(std::chrono::steady_clock::duration duration);

std::vector<QString>& defaultPluginDirectoryList();

void locateFileInExplorer(const QString& path);
}

#endif // YADAW_NATIVE_NATIVE
