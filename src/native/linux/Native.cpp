#if __linux__

#include "native/Native.hpp"
#include "util/Base.hpp"

#include <QProcess>

#include <signal.h>
#include <time.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iterator>
#include <mutex>

namespace YADAW::Native
{
const QString& appDataFolder()
{
    static auto ret = QString(std::getenv("HOME")) + "/.local/share";
    return ret;
}

void openSpecialCharacterInput()
{
    std::vector<QString> listOfCharMap;
    auto desktop = std::getenv("XDG_CURRENT_DESKTOP");
    if(std::strstr(desktop, "KDE"))
    {
        listOfCharMap.emplace_back("kcharselect");
        listOfCharMap.emplace_back("gucharmap");
    }
    else
    {
        listOfCharMap.emplace_back("gucharmap");
        listOfCharMap.emplace_back("kcharselect");
    }
    QProcess process;
    for(const auto& charmap: listOfCharMap)
    {
        if(QProcess::startDetached(charmap))
        {
            break;
        }
    }
}

bool isDebuggerPresent()
{
    const auto pid = getpid();
    const auto size =
        YADAW::Util::stringLength("/proc/")
        + 11 // length of INT32_MAX as string
        + YADAW::Util::stringLength("/status")
        + 1;
    std::vector<char> path(size, '\0');
    std::sprintf(path.data(), "/proc/%d/status", pid);
    std::ifstream ifs(path.data());
    char lineBuffer[128];
    std::memset(lineBuffer, 0, std::size(lineBuffer));
    while(ifs.peek() != EOF)
    {
        ifs.getline(lineBuffer, std::size(lineBuffer));
        int tracerPid = 0;
        if(std::sscanf(lineBuffer, "TracerPid:\t%d", &tracerPid) != 0)
        {
            return tracerPid != 0;
        }
    }
    return false;
}

void sleepFor(std::chrono::steady_clock::duration duration)
{
    auto nanosecond = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    timespec timespec;
    timespec.tv_sec = nanosecond / 1000000000;
    timespec.tv_nsec = nanosecond - timespec.tv_sec * 1000000000;
    nanosleep(&timespec, nullptr);
}

std::once_flag defaultPluginListFlag;

const std::vector<QString>& defaultPluginDirectoryList()
{
    static std::vector<QString> ret;
    std::call_once(defaultPluginListFlag,
        [&list = ret]() mutable
        {
            list.reserve(6);
            // VST3
            list.emplace_back(QString(std::getenv("HOME")) + "/.vst3");
            list.emplace_back("/usr/lib/vst3");
            list.emplace_back("/usr/local/lib/vst3");
            // CLAP
            list.emplace_back(QString(std::getenv("HOME")) + "/.clap");
            list.emplace_back("/usr/lib/clap");
            auto clapPath = std::getenv("CLAP_PATH");
            if(clapPath)
            {
                QString clapPathAsQString(clapPath);
                auto results = clapPathAsQString.split(':');
                for(const auto& result: results)
                {
                    if(result.size() != 0)
                    {
                        list.emplace_back(result);
                    }
                }
            }
        }
    );
    return ret;
}

QString getFileBrowserName()
{
    auto desktop = std::getenv("XDG_CURRENT_DESKTOP");
    if(std::strstr(desktop, "KDE"))
    {
        return "Plasma";
    }
    if(std::strstr(desktop, "GNOME"))
    {
        return "Nautilus";
    }
    if(std::strstr(desktop, "XFCE"))
    {
        return "Thunar";
    }
    return "";
}

void mySegFaultHandler()
{
    std::fprintf(stderr, "Access violation");
    std::terminate();
}

void segFaultHandler(int sig, siginfo_t* si, void* unused)
{
    auto* uc = reinterpret_cast<ucontext_t*>(unused);
    auto& gregs = uc->uc_mcontext.gregs;
    gregs[REG_RIP] = reinterpret_cast<greg_t>(&mySegFaultHandler);
    gregs[REG_RSP] -= sizeof(void*);
    *reinterpret_cast<greg_t*>(gregs[REG_RSP]) = gregs[REG_RIP];
}

bool setBadMemoryAccessHandler()
{
    struct sigaction sigAction;
    sigAction.sa_flags = SA_SIGINFO;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_sigaction = &segFaultHandler;
    return sigaction(SIGSEGV, &sigAction, NULL) == 0;
}
}

#endif
