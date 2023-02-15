#ifndef YADAW_SRC_AUDIO_UTIL_CLAPUTIL
#define YADAW_SRC_AUDIO_UTIL_CLAPUTIL

#include <clap/plugin.h>

template<typename To>
void getExtension(const clap_plugin* plugin, const char* id, To** to)
{
    *to = reinterpret_cast<To*>(plugin->get_extension(plugin, id));
}

#endif //YADAW_SRC_AUDIO_UTIL_CLAPUTIL
