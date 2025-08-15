#ifndef YADAW_PCH_CLAP
#define YADAW_PCH_CLAP

#include <clap/events.h>
#include <clap/host.h>
#include <clap/entry.h>
#include <clap/plugin.h>
#include <clap/plugin-features.h>
#include <clap/ext/ambisonic.h>
#include <clap/ext/audio-ports.h>
#include <clap/ext/audio-ports-config.h>
#include <clap/ext/gui.h>
#include <clap/ext/note-ports.h>
#include <clap/ext/params.h>
#include <clap/ext/surround.h>
#include <clap/ext/tail.h>
#include <clap/ext/timer-support.h>
#include <clap/factory/plugin-factory.h>

#if __linux__
#include <clap/ext/posix-fd-support.h>
#endif

#endif // YADAW_PCH_CLAP
