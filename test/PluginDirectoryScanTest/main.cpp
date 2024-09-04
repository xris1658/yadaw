#include "controller/PluginController.hpp"

#include <QDir>

#include <cstdio>

int main(int argc, char** argv)
{
    if(argc == 1 || argc == 0)
    {
        std::printf("Usage: PluginDirectoryScanTest [path] <more paths>");
        return 0;
    }
    for(int i = 1; i < argc; ++i)
    {
        const auto& list = YADAW::Controller::scanDirectory(
            QDir(QString::fromLocal8Bit(argv[i])), true, true);
        for(const auto& item: list)
        {
            auto results = YADAW::Controller::scanSingleLibraryFile(item);
            {
                auto itemString = item.toLocal8Bit();
                std::wprintf(
                    L"Complete scanning %s, %d results found\n",
                    itemString.data(), static_cast<int>(results.size())
                );
            }
            int j = 1;
            for(auto& [pluginInfo, pluginCategories]: results)
            {
                auto name = pluginInfo.name.toLocal8Bit();
                auto vendor = pluginInfo.vendor.toLocal8Bit();
                auto version = pluginInfo.version.toLocal8Bit();
                std::printf("%d:\n", j++);
                std::printf("   Name: %s\n"
                            " Vendor: %s\n"
                            "Version: %s\n"
                            "   Type: ",
                    name.data(), vendor.data(), version.data()
                );
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
                std::printf(" Format: ");
                if(pluginInfo.format == YADAW::DAO::PluginFormatVST3)
                {
                    std::printf("VST3\n");
                }
                else if(pluginInfo.format == YADAW::DAO::PluginFormatCLAP)
                {
                    std::printf("CLAP\n");
                }
                else if(pluginInfo.format == YADAW::DAO::PluginFormatVestifal)
                {
                    std::printf("Vestifal\n");
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
                        auto tagString = tag.toLocal8Bit();
                        std::printf("%s", tagString.data());
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
    std::printf("Scan complete. Press any key to continue...");
    std::getchar();
    return 0;
}
