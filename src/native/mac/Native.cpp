#if __APPLE__

#include "native/Native.hpp"

#include <QCoreApplication>
#include <QString>

#if TARGET_CPU_X86
#include <cpuid.h>
#endif

#include <mutex>

namespace YADAW::Native
{
const QString& appDataFolder()
{
    static QString ret = QString(std::getenv("HOME")) + "/Library";
    return ret;
}

const QString& programFilesFolder()
{
    static QString ret;
    return ret;
}

void openSpecialCharacterInput()
{
}

int getProcessCPUCoreCount()
{
    return 16;
}

void setThreadPriorityToTimeCritical()
{
}

bool isDebuggerPresent()
{
    return false;
}

QString errorMessageFromErrorCode(ErrorCodeType errorCode)
{
    return {};
}

QString getProductVersion(const QString& path)
{
    return {};
}

void sleepFor(std::chrono::steady_clock::duration duration)
{
}

std::once_flag defaultPluginListFlag;

const std::vector<QString>& defaultPluginDirectoryList()
{
    static std::vector<QString> ret;
    std::call_once(
        defaultPluginListFlag, [&list = ret]() mutable
        {
            list.reserve(7);
            auto systemPluginPath = QString("/Library/Audio/Plug-Ins");
            // AudioUnit
            list.emplace_back(QString(systemPluginPath).append("/Components"));
            // Vestifal
            list.emplace_back(QString(systemPluginPath).append("/VST"));
            // VST3
            list.emplace_back(QString(systemPluginPath).append("/VST3"));
            // CLAP
            list.emplace_back(QString(systemPluginPath).append("/CLAP"));
            auto userPluginPath = QString(std::getenv("HOME")) + "/Library/Audio/Plug-Ins";
            // Vestifal
            list.emplace_back(QString(userPluginPath).append("/VST"));
            // VST3
            list.emplace_back(QString(userPluginPath).append("/VST3"));
            // CLAP
            list.emplace_back(QString(userPluginPath).append("/CLAP"));
        }
    );
    return ret;
}

bool setBadMemoryAccessHandler()
{
    return false;
}

std::uint32_t cpuidInfo[4];

void fillCPUIDInfo()
{
#if TARGET_CPU_X86
    __get_cpuid(1, cpuidInfo, cpuidInfo + 1, cpuidInfo + 2, cpuidInfo + 3);
#endif
}
}

#endif