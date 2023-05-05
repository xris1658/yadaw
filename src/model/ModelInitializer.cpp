#include "ModelInitializer.hpp"

#include "util/QmlUtil.hpp"
#include "model/IAssetDirectoryListModel.hpp"
#include "model/IAudioBusChannelListModel.hpp"
#include "model/IAudioBusConfigurationModel.hpp"
#include "model/IAudioGraphDeviceListModel.hpp"
#include "model/IAutomationModel.hpp"
#include "model/ILocalizationListModel.hpp"
#include "model/IPluginDirectoryListModel.hpp"
#include "model/IPluginListModel.hpp"

namespace YADAW::Model
{
void initializeModel()
{
    QML_REGISTER_TYPE(IAssetDirectoryListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioBusChannelListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioBusConfigurationModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioGraphDeviceListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAutomationModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(ILocalizationListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IPluginDirectoryListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IPluginListModel, YADAW.Models, 1, 0);
}
}