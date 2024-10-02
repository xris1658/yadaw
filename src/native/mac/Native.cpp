#if __APPLE__

#include "native/Native.hpp"

#include "native/Library.hpp"

#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h>
#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFBundle.h>
#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFString.h>

#include <QCoreApplication>
#include <QString>

#include <cpuid.h>

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

QString fileBrowserName;

std::once_flag getFileBrowserNameFlag;

QString getFileBrowserName()
{
    std::call_once(getFileBrowserNameFlag,
        []()
        {
            auto finderAppPath = CFSTR("/System/Library/CoreServices/Finder.app");
            auto finderAppUrl = CFURLCreateWithFileSystemPath(
                kCFAllocatorDefault, finderAppPath, kCFURLPOSIXPathStyle, true
            );
            auto bundle = CFBundleCreate(kCFAllocatorDefault, finderAppUrl);
            if(bundle)
            {
                auto defaultName = CFSTR("Finder");
                auto infoPlist = CFSTR("InfoPlist");
                auto key = CFSTR("CFBundleDisplayName");
                auto name = CFBundleCopyLocalizedString(
                    bundle, key,
                    defaultName,
                    infoPlist
                );
                CFRelease(infoPlist);
                CFRelease(defaultName);
                CFRelease(key);
                fileBrowserName = QString::fromCFString(name);
                CFRelease(bundle);
            }
            else
            {
                fileBrowserName = "Finder";
            }
        }
    );
    return fileBrowserName;
}

void locateFileInExplorer(const QString& path)
{
    auto command = QString("open -R %1").arg(path).toLocal8Bit();
    std::system(command.constData());
}

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