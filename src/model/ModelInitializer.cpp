#include "ModelInitializer.hpp"

#include "util/QmlUtil.hpp"
#include "model/IAssetDirectoryListModel.hpp"
#include "model/IPluginDirectoryListModel.hpp"

namespace YADAW::Model
{
void initializeModel()
{
    QML_REGISTER_TYPE(IAssetDirectoryListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IPluginDirectoryListModel, YADAW.Models, 1, 0);
}
}