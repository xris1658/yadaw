#include "entity/PluginFormatSupport.hpp"

namespace YADAW::Entity
{
PluginFormatSupport::PluginFormatSupport(QObject* parent): QObject(parent) {}

bool PluginFormatSupport::isPluginFormatSupported(
    PluginFormatSupport::PluginFormat pluginFormat) const
{
    // TODO: Rewrite this if new plugin support is added
    return true;
}
}