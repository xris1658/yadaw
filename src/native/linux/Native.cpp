#if(__linux__)

#include "native/Native.hpp"
#include "util/Base.hpp"

#include <time.h>
#include <unistd.h>

#include <ctime>
#include <mutex>

namespace YADAW::Native
{
const QString& appDataFolder()
{
    static auto ret = QString(std::getenv("HOME")) + "/.local";
    return ret;
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
    std::memset(lineBuffer, 0, YADAW::Util::stackArraySize(lineBuffer));
    while(ifs.peek() != EOF)
    {
        ifs.getline(lineBuffer, YADAW::Util::stackArraySize(lineBuffer));
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

std::int64_t currentTimeValueInNanosecond()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> currentTimePointInNanosecond()
{
    return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now());
}

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

void locateFileInExplorer(const QString& path)
{
    // GNOME:      nautilus -s [URI]
    // Xfce:       thunar [URI]
    // KDE Plasma: dolphin [URI] --new-window --select
    // LXDE:       Not supported by pcmanfm?
    // Cinnamon:   nemo [URI]
}
}

#endif