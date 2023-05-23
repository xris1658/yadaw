#if(__linux__)

#include "native/Native.hpp"

#include <time.h>

#include <ctime>
#include <mutex>

namespace YADAW::Native
{
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
            list.emplace_back("$HOME/.vst3");
            list.emplace_back("/usr/lib/vst3");
            list.emplace_back("/usr/local/lib/vst3");
            // CLAP
            list.emplace_back("$HOME/.clap");
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