#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINCONTROLLER

#include "audio/engine/AudioDeviceProcess.hpp"
#include "audio/host/CLAPEventList.hpp"
#include "audio/host/CLAPHost.hpp"
#include "audio/plugin/IAudioPlugin.hpp"
#include "controller/LibraryPluginMap.hpp"
#include "controller/PluginContext.hpp"
#include "dao/PluginTable.hpp"
#include "dao/PluginCategoryTable.hpp"
#include "native/Library.hpp"

#include <QDir>
#include <QString>

#include <vector>

namespace YADAW::Controller
{
using namespace YADAW::Audio::Plugin;

struct PluginAndProcess
{
    PluginAndProcess() = default;
    PluginAndProcess(YADAW::Audio::Plugin::IAudioPlugin* plugin,
        YADAW::Audio::Engine::AudioDeviceProcess process):
        plugin(plugin), process(process)
    {}
    PluginAndProcess(PluginAndProcess&&) noexcept = default;
    YADAW::Audio::Plugin::IAudioPlugin* plugin;
    YADAW::Audio::Engine::AudioDeviceProcess process;
};

struct PluginScanResult
{
    YADAW::DAO::PluginInfo pluginInfo;
    std::vector<QString> pluginCategories;
};

std::vector<QString> scanDirectory(const QDir& dir, bool recursive, bool includeSymLink);

std::vector<PluginScanResult> scanSingleLibraryFile(const QString& path);

void savePluginScanResult(const PluginScanResult& result);

std::optional<
    std::pair<
        YADAW::Controller::LibraryPluginMap::iterator,
        PluginAndProcess
    >
> createPlugin(
    const QString& path, YADAW::DAO::PluginFormat format,
    const std::vector<char>& uid);

using FillPluginContextCallback = bool(PluginContext& context);

struct CLAPHostContext
{
    YADAW::Audio::Host::CLAPHost host;
    YADAW::Audio::Host::CLAPEventList eventList;
    CLAPHostContext(YADAW::Audio::Plugin::CLAPPlugin& clapPlugin);
};

// Load a plugin instance from the description (path, format and uid),
// create a `PluginContext` with its `pluginInstance` and `process` filled,
// and invoke the callback to fill other parts of the `PluginContext` before
// returning it.
std::optional<PluginContext> createPluginWithContext(
    const QString& path, YADAW::DAO::PluginFormat format,
    const std::vector<char>& uid,
    const std::function<FillPluginContextCallback>& callback);
}

#endif // YADAW_SRC_CONTROLLER_PLUGINCONTROLLER
