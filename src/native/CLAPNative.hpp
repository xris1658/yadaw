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

std::shared_ptr<YADAW::Audio::Plugin::CLAPPlugin> createCLAPFromLibrary(const QString& path);
}

#endif //YADAW_SRC_NATIVE_CLAPNATIVE
