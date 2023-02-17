#include "audio/plugin/VST3Plugin.hpp"
#include "controller/PluginController.hpp"
#include "native/Library.hpp"

#include <cstdio>

int main(int argc, char** argv)
{
    if(argc == 1 || argc == 0)
    {
        std::wprintf(L"Usage: PluginScanTest [path] <more paths>");
        return 0;
    }
    for(int i = 1; i < argc; ++i)
    {
        auto results = YADAW::Controller::scanSingleLibraryFile(argv[i]);
        std::printf("Complete scanning %s, %d results found\n", argv[i], static_cast<int>(results.size()));
        int j = 1;
        for(auto& [pluginInfo, pluginCategories]: results)
        {
            std::printf("%d:\n", j++);
            std::wprintf(L"   Name: %s\n"
                          " Vendor: %s\n"
                          "Version: %s\n"
                          "   Type: ",
                          reinterpret_cast<const wchar_t*>(pluginInfo.name.data()),
                          reinterpret_cast<const wchar_t*>(pluginInfo.vendor.data()),
                          reinterpret_cast<const wchar_t*>(pluginInfo.version.data()));
            if(pluginInfo.type == YADAW::DAO::PluginTypeInstrument)
            {
                std::printf("Instrument\n");
            }
            else if(pluginInfo.type == YADAW::DAO::PluginTypeAudioEffect)
            {
                std::printf("Audio Effect\n");
            }
            else if(pluginInfo.type == YADAW::DAO::PluginTypeAudioEffect)
            {
                std::printf("MIDI Effect\n");
            }
            else
            {
                std::printf("Unknown\n");
            }
            std::printf("%d Tags", static_cast<int>(pluginCategories.size()));
            if(!pluginCategories.empty())
            {
                std::printf(":\n");
                for(const auto& tag: pluginCategories)
                {
                    std::wprintf(L"%s", reinterpret_cast<const wchar_t*>(tag.data()));
                    if(&tag != &(pluginCategories.back()))
                    {
                        std::printf(" / ");
                    }
                }
            }
            std::printf("\n-----------------------------------------------------------------\n");
        }
    }
}