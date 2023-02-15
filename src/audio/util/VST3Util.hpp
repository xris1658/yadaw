#ifndef YADAW_SRC_AUDIO_UTIL_VST3UTIL
#define YADAW_SRC_AUDIO_UTIL_VST3UTIL

#include <pluginterfaces/base/funknown.h>

#include <type_traits>

template<typename To>
Steinberg::tresult queryInterface(Steinberg::FUnknown* from, To** to)
{
    static_assert(std::is_base_of_v<Steinberg::FUnknown, To>);
    return from->queryInterface(To::iid, reinterpret_cast<void**>(to));
}

#endif //YADAW_SRC_AUDIO_UTIL_VST3UTIL
