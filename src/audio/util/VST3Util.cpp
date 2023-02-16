#include "VST3Util.hpp"

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
            return splittedSubCategoryCount;
        }
    }
}
}
