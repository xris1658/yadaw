#ifndef YADAW_SRC_CONTROLLER_PLUGINCONTROLLER
#define YADAW_SRC_CONTROLLER_PLUGINCONTROLLER

#include "audio/plugin/CLAPPlugin.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "dao/PluginTable.hpp"
#include "dao/PluginCategoryTable.hpp"
#include "native/Library.hpp"

#include <QDir>
#include <QString>

#include <vector>

namespace YADAW::Controller
{
using namespace YADAW::Audio::Plugin;

struct PluginScanResult
{
    YADAW::DAO::PluginInfo pluginInfo;
    std::vector<QString> pluginCategories;
};

std::vector<QString> scanDirectory(const QDir& dir, bool recursive, bool includeSymLink);

std::vector<PluginScanResult> scanSingleLibraryFile(const QString& path);

void savePluginScanResult(const PluginScanResult& result);
}

#endif //YADAW_SRC_CONTROLLER_PLUGINCONTROLLER
