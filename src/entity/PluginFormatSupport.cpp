#include "entity/PluginFormatSupport.hpp"

#include "util/QmlUtil.hpp"

namespace YADAW::Entity
{
PluginFormatSupport::PluginFormatSupport(QObject* parent): QObject(parent)
{
    YADAW::Util::setCppOwnership(*this);
}

bool PluginFormatSupport::isPluginFormatSupported(
    PluginFormatSupport::PluginFormat pluginFormat) const
{
    // TODO: Rewrite this if new plugin support is added
    return true;
}
}
