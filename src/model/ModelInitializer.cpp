#include "ModelInitializer.hpp"

#include "util/QmlUtil.hpp"
#include "model/IAssetDirectoryListModel.hpp"
#include "model/IAudioBusChannelListModel.hpp"
#include "model/IAudioBusConfigurationModel.hpp"
#include "model/IAudioChannelGroupModel.hpp"
#include "model/IAudioDeviceListModel.hpp"
#include "model/IAutomationModel.hpp"
#include "model/IFilterRoleModel.hpp"
#include "model/ILocalizationListModel.hpp"
#include "model/IPluginDirectoryListModel.hpp"
#include "model/IPluginListModel.hpp"
#include "model/ISortFilterListModel.hpp"
#include "model/ISortFilterProxyListModel.hpp"
#include "model/ISortOrderModel.hpp"

namespace YADAW::Model
{
void initializeModel()
{
    QML_REGISTER_TYPE(IAssetDirectoryListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioBusChannelListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioBusConfigurationModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioChannelGroupModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioDeviceListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAutomationModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IFilterRoleModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(ILocalizationListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IPluginDirectoryListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IPluginListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(ISortFilterListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(ISortFilterProxyListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(ISortOrderModel, YADAW.Models, 1, 0);
}
}