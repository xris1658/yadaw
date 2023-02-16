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

VST3Plugin createVST3FromLibrary(Library& library)
{
    auto factory = library.getExport<VST3Plugin::FactoryEntry>(factoryEntryName);
    if(!factory)
    {
        return {};
    }
    auto init = library.getExport<VST3Plugin::InitEntry>(initEntryName);
    auto exit = library.getExport<VST3Plugin::ExitEntry>(exitEntryName);
    return VST3Plugin(init, factory, exit);
}
}