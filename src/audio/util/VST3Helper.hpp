#ifndef YADAW_SRC_AUDIO_UTIL_VST3HELPER
#define YADAW_SRC_AUDIO_UTIL_VST3HELPER

#include "audio/base/Automation.hpp"
#include "audio/host/VST3ParameterValueQueue.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "native/Library.hpp"
#if __cplusplus >= 202002L
#include "util/Concepts.hpp"
#else
#include <type_traits>
#endif

#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/base/ipluginbase.h>

#if __cplusplus >= 202002L
template<DerivedTo<Steinberg::FUnknown> To>
inline Steinberg::tresult queryInterface(Steinberg::FUnknown* from, To** to)
{
    return from->queryInterface(To::iid, reinterpret_cast<void**>(to));
}

template<DerivedTo<Steinberg::FUnknown> T>
inline void releasePointer(T*& pointer)
{
    if(pointer)
    {
        pointer->release();
        pointer = nullptr;
    }
}
#else
template<typename To>
inline Steinberg::tresult queryInterface(Steinberg::FUnknown* from, To** to)
{
    static_assert(std::is_base_of_v<Steinberg::FUnknown, To>);
    return from->queryInterface(To::iid, reinterpret_cast<void**>(to));
}

template<typename T>
inline void releasePointer(T*& pointer)
{
    static_assert(std::is_base_of_v<Steinberg::FUnknown, T>);
    if(pointer)
    {
        pointer->release();
        pointer = nullptr;
    }
}
#endif

namespace YADAW::Audio::Util
{
using SubCategories = decltype(Steinberg::PClassInfo2::subCategories);
using SplittedSubCategories = char*[Steinberg::PClassInfo2::kSubCategoriesSize];
int splitSubCategories(SubCategories& subCategories, SplittedSubCategories& splittedSubCategories);

YADAW::Audio::Plugin::VST3Plugin createVST3FromLibrary(const YADAW::Native::Library& library);

bool fillAutomationInParamValueQueue(
    const YADAW::Audio::Base::Automation& automation,
    YADAW::Audio::Host::VST3ParameterValueQueue& emptyQueue,
    YADAW::Audio::Base::Automation::Time from,
    YADAW::Audio::Base::Automation::Time length,
    YADAW::Audio::Base::Automation::Time precision = 8);
}

#endif // YADAW_SRC_AUDIO_UTIL_VST3HELPER
