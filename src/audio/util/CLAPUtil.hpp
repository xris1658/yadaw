#ifndef YADAW_SRC_AUDIO_UTIL_CLAPUTIL
#define YADAW_SRC_AUDIO_UTIL_CLAPUTIL

#include "audio/plugin/CLAPPlugin.hpp"
#include "native/Library.hpp"

#include <clap/plugin.h>

#include <algorithm>

namespace Impl
{
template<typename T, typename... U>
struct MaxSizeHelper
{
    static constexpr std::size_t value = std::max(sizeof(T), MaxSizeHelper<U...>::value);
};

template<typename T>
struct MaxSizeHelper<T>
{
    static constexpr std::size_t value = sizeof(T);
};
}

template<typename... T>
constexpr std::size_t MaxSize = Impl::MaxSizeHelper<T...>::value;

template<typename To>
void getExtension(const clap_plugin* plugin, const char* id, To** to)
{
    *to = reinterpret_cast<To*>(plugin->get_extension(plugin, id));
}

namespace YADAW::Audio::Util
{
YADAW::Audio::Plugin::CLAPPlugin createCLAPFromLibrary(YADAW::Native::Library& library);
}

#endif //YADAW_SRC_AUDIO_UTIL_CLAPUTIL
