#ifndef YADAW_SRC_AUDIO_UTIL_VST3UTIL
#define YADAW_SRC_AUDIO_UTIL_VST3UTIL

#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/base/ipluginbase.h>

#include <type_traits>

template<typename To>
Steinberg::tresult queryInterface(Steinberg::FUnknown* from, To** to)
{
    static_assert(std::is_base_of_v<Steinberg::FUnknown, To>);
    return from->queryInterface(To::iid, reinterpret_cast<void**>(to));
}

template<typename T>
void releasePointer(T* pointer)
{
    static_assert(std::is_base_of_v<Steinberg::FUnknown, T>);
    if(pointer)
    {
        pointer->release();
        pointer = nullptr;
    }
}

namespace YADAW::Audio::Util
{
using SubCategories = decltype(Steinberg::PClassInfo2::subCategories);
using SplittedSubCategories = char*[Steinberg::PClassInfo2::kSubCategoriesSize];
int splitSubCategories(SubCategories& subCategories, SplittedSubCategories& splittedSubCategories);
}


#endif //YADAW_SRC_AUDIO_UTIL_VST3UTIL
