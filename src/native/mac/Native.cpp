#if __APPLE__

#include "native/Native.hpp"

#include <QCoreApplication>

#include <cpuid.h>

#include <mutex>

namespace YADAW::Native
{
const QString& appDataFolder()
{
    static QString ret = QCoreApplication::applicationDirPath() + "/AppData";
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

void showFileInExplorer(const QString& path)
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
            list.reserve(6);
            auto systemPluginPath = QString("/Library/Audio/Plug-Ins");
            // AudioUnit
            list.emplace_back(QString(systemPluginPath).append("Components"));
            // Vestifal
            list.emplace_back(QString(systemPluginPath).append("/VST"));
            // VST3
            list.emplace_back(QString(systemPluginPath).append("/VST3"));
            // CLAP
            list.emplace_back(QString(systemPluginPath).append("/CLAP"));
            auto userName = std::getenv("USERNAME");
            if(userName)
            {
                auto userPluginPath = QString("/Users/%1/Library/Audio/Plug-Ins").arg(userName);
                // Vestifal
                list.emplace_back(QString(userPluginPath).append("/VST"));
                // VST3
                list.emplace_back(QString(userPluginPath).append("/VST3"));
                // CLAP
                list.emplace_back(QString(userPluginPath).append("/CLAP"));

            }
        }
    );
    return ret;
}

void locateFileInExplorer(const QString& path)
{}

bool setBadMemoryAccessHandler()
{
    return false;
}

std::uint32_t cpuidInfo[4];

void fillCPUIDInfo()
{
    __get_cpuid(1, cpuidInfo, cpuidInfo + 1, cpuidInfo + 2, cpuidInfo + 3);
}
}

#endif