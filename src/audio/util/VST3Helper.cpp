#include "VST3Helper.hpp"

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
    auto init = library.getExport<YADAW::Native::VST3InitEntry>(YADAW::Native::initEntryName);
    auto exit = library.getExport<YADAW::Audio::Plugin::VST3Plugin::ExitEntry>(YADAW::Native::exitEntryName);
    return {init, factory, exit, library.handle()};
}

bool fillAutomationInParamValueQueue(
    const YADAW::Audio::Base::Automation& automation,
    YADAW::Audio::Host::VST3ParameterValueQueue& queue,
    YADAW::Audio::Base::Automation::Time from,
    YADAW::Audio::Base::Automation::Time length,
    YADAW::Audio::Base::Automation::Time precision)
{
    if(queue.getPointCount() == 0 && precision != 0)
    {
        Steinberg::int32 index;
        if(automation.empty())
        {
            queue.addPoint(0, automation.defaultValue(), index);
        }
        else if(automation.pointCount() == 1)
        {
            queue.addPoint(0, automation[0].value(), index);
        }
        else
        {
            auto to = from + length;
            auto fromLowerBound = automation.lowerBound(from);
            auto toLowerBound = automation.lowerBound(to);
            // ????[---)-o--
            if(fromLowerBound == toLowerBound)
            {
                // ----[---)-o--
                if(fromLowerBound == automation.cbegin())
                {
                    queue.addPoint(0, automation[0].value(), index);
                }
                // --o-[----)-o--
                else
                {
                    auto beforeBegin = fromLowerBound - 1;
                    if(beforeBegin->value() == fromLowerBound->value())
                    {
                        queue.addPoint(0, fromLowerBound->value(), index);
                    }
                    else
                    {
                        auto function = automation.getFunction(beforeBegin);
                        if(function.a == 0)
                        {
                            assert(function.b != 0);
                            queue.addPoint(0, function(from), index);
                            queue.addPoint(length - 1, function(from + length - 1), index);
                        }
                        else
                        {
                            auto i = 0;
                            while(i < length)
                            {
                                queue.addPoint(i, function(from + i), index);
                                i += precision;
                            }
                        }
                    }
                }
            }
            // ????[-o-)-o--
            else
            {
                // ----[-o-)-o--
                if(fromLowerBound == automation.cbegin())
                {
                    queue.addPoint(0, automation[0].value(), index);
                }
                // -o--[-o-)-o--
                else
                {
                    auto beforeBegin = fromLowerBound - 1;
                    if(beforeBegin->value() == fromLowerBound->value())
                    {
                        queue.addPoint(0, fromLowerBound->value(), index);
                    }
                    else
                    {
                        auto function = automation.getFunction(beforeBegin);
                        if(function.a == 0)
                        {
                            assert(function.b != 0);
                            queue.addPoint(0, function(from), index);
                            queue.addPoint(fromLowerBound->time() - from,
                                fromLowerBound->value(), index);
                        }
                        else
                        {
                            auto i = 0;
                            while(i < fromLowerBound->time())
                            {
                                queue.addPoint(i, function(from + i), index);
                                i += precision;
                            }
                        }
                    }
                    auto it = fromLowerBound;
                    while(it + 1 != toLowerBound)
                    {
                        auto function = automation.getFunction(it);
                        if(function.a == 0)
                        {
                            queue.addPoint(it->time() - from,
                                it->value(), index);
                        }
                        else
                        {
                            auto i = 0;
                            auto duration = (it + 1)->time() - it->time();
                            while(i < duration)
                            {
                                queue.addPoint(it->time() + i - from,
                                    function(it->time() + i), index);
                                i += precision;
                            }
                        }
                        ++it;
                    }
                    auto function = automation.getFunction(it);
                    if(function.a == 0)
                    {
                        queue.addPoint(it->time() - from, it->value(), index);
                    }
                    else
                    {
                        auto i = 0;
                        auto duration = from + length - it->time();
                        while(i < duration)
                        {
                            queue.addPoint(it->time() + i - from,
                                function(it->time() + i), index);
                            i += precision;
                        }
                    }
                }
            }
        }
        return true;
    }
    return false;
}
}
