#ifndef YADAW_SRC_NATIVE_CLAPNATIVE
#define YADAW_SRC_NATIVE_CLAPNATIVE

#include "audio/plugin/CLAPPlugin.hpp"
#include "native/Library.hpp"

#include <clap/ext/gui.h>

#include <QWindow>

#include <memory>

namespace YADAW::Native
{
extern const char* windowAPI;

void setWindow(clap_window& clapWindow, QWindow* window);

YADAW::Audio::Plugin::CLAPPlugin createCLAPFromLibrary(Library& library);
}

#endif //YADAW_SRC_NATIVE_CLAPNATIVE
