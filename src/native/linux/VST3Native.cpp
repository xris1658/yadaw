#include "native/VST3Native.hpp"

namespace YADAW::Native
{
using YADAW::Native::Library;
using YADAW::Audio::Plugin::VST3Plugin;

const Steinberg::FIDString ViewType = Steinberg::kPlatformTypeX11EmbedWindowID;

const char* initEntryName = "ModuleEntry";
const char* exitEntryName = "ModuleExit";
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
    if(init && exit)
    {
        return VST3Plugin(init, factory, exit);
    }
    return {};
}
}