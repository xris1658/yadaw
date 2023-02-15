#include "VST3Native.hpp"

#include "native/Library.hpp"

namespace YADAW::Native
{
using YADAW::Native::Library;
using YADAW::Audio::Plugin::VST3Plugin;

const Steinberg::FIDString ViewType = Steinberg::kPlatformTypeHWND;

const char* initEntryName = "InitDll";
const char* exitEntryName = "ExitDll";
const char* factoryEntryName = "GetPluginFactory";

std::shared_ptr<VST3Plugin> createVST3FromLibrary(const QString& path)
{
    auto library = std::make_shared<Library>(path);
    if(!library)
    {
        return nullptr;
    }
    auto factory = library->getExport<VST3Plugin::FactoryEntry>(factoryEntryName);
    if(!factory)
    {
        return nullptr;
    }
    auto init = library->getExport<VST3Plugin::InitEntry>(initEntryName);
    auto exit = library->getExport<VST3Plugin::ExitEntry>(exitEntryName);
    auto plugin = new VST3Plugin(init, factory, exit);
    return {
        plugin,
        [library = std::move(library)](void* ptr)
        {
            auto ptrAsVST3Plugin = static_cast<VST3Plugin*>(ptr);
            delete ptrAsVST3Plugin;
        }
    };
}
}