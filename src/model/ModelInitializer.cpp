#include "ModelInitializer.hpp"

#include "util/QmlUtil.hpp"
#include "model/AssetDirectoryListModel.hpp"
#include "model/PluginDirectoryListModel.hpp"

namespace YADAW::Model
{
void initializeModel()
{
    QML_REGISTER_TYPE(AssetDirectoryListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(PluginDirectoryListModel, YADAW.Models, 1, 0);
}
}