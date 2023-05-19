#ifndef YADAW_PCH_LINUXAPI
#define YADAW_PCH_LINUXAPI

// Linux API
#if(__linux__)
#include <dlfcn.h>
#include <time.h>

// ALSA
#include <alsa/asoundlib.h>
#endif

#endif //YADAW_PCH_LINUXAPI
