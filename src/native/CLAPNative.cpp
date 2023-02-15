#include "CLAPNative.hpp"

#include <clap/entry.h>
#include <clap/ext/gui.h>

namespace YADAW::Native
{
using YADAW::Native::Library;
using YADAW::Audio::Plugin::CLAPPlugin;

const char* windowAPI = CLAP_WINDOW_API_WIN32;

void setWindow(clap_window& clapWindow, QWindow* window)
{
    clapWindow.win32 = reinterpret_cast<decltype(clapWindow.win32)>(window->winId());
}

std::shared_ptr<YADAW::Audio::Plugin::CLAPPlugin> YADAW::Native::createCLAPFromLibrary(const QString& path)
{
    auto library = std::make_shared<Library>(path);
    if(!library)
    {
        return nullptr;
    }
    auto plugin = new CLAPPlugin(library->getExport<const clap_plugin_entry*>("clap_entry"));
    return {
        plugin,
        [library = std::move(library)](void* ptr)
        {
            auto ptrAsCLAPPlugin = static_cast<CLAPPlugin*>(ptr);
            delete ptrAsCLAPPlugin;
        }
    };
}

}