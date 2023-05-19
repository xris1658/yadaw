#include "VST3Util.hpp"

#include "native/VST3Native.hpp"

#include <cstring>

namespace YADAW::Audio::Util
{
int splitSubCategories(SubCategories& subCategories, SplittedSubCategories& splittedSubCategories)
{
    std::memset(splittedSubCategories, 0, Steinberg::PClassInfo2::kSubCategoriesSize);
    int splittedSubCategoryCount = 0;
    int indexToAppend = 0;
    for(int i = 0; i < Steinberg::PClassInfo2::kSubCategoriesSize; ++i)
    {
        if(subCategories[i] == '|')
        {
            subCategories[i] = '\0';
            if(indexToAppend != i)
            {
                splittedSubCategories[splittedSubCategoryCount++] = subCategories + indexToAppend;
            }
            indexToAppend = i + 1;
        }
        else if(subCategories[i] == '\0')
        {
            if(indexToAppend != i)
            {
                splittedSubCategories[splittedSubCategoryCount++] = subCategories + indexToAppend;
            }
            break;
        }
    }
    return splittedSubCategoryCount;
}

YADAW::Audio::Plugin::VST3Plugin createVST3FromLibrary(Native::Library& library)
{
    auto factory = library.getExport<YADAW::Audio::Plugin::VST3Plugin::FactoryEntry>("GetPluginFactory");
    if(!factory)
    {
        return {};
    }
    auto init = library.getExport<YADAW::Audio::Plugin::VST3Plugin::InitEntry>(YADAW::Native::initEntryName);
    auto exit = library.getExport<YADAW::Audio::Plugin::VST3Plugin::ExitEntry>(YADAW::Native::exitEntryName);
    return {init, factory, exit};
}
}
