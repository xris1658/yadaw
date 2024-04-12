#include "ModelInitializer.hpp"

#include "util/QmlUtil.hpp"
#include "model/IAssetDirectoryListModel.hpp"
#include "model/IAudioBusChannelListModel.hpp"
#include "model/IAudioBusConfigurationModel.hpp"
#include "model/IAudioDeviceIOGroupListModel.hpp"
#include "model/IAudioDeviceListModel.hpp"
#include "model/IAutomationModel.hpp"
#include "model/IFileTreeModel.hpp"
#include "model/ILocalizationListModel.hpp"
#include "model/IMixerChannelInsertListModel.hpp"
#include "model/IMixerChannelListModel.hpp"
#include "model/IParameterValueAndTextListModel.hpp"
#include "model/IPluginCategoryListModel.hpp"
#include "model/IPluginDirectoryListModel.hpp"
#include "model/IPluginListModel.hpp"
#include "model/IPluginParameterListModel.hpp"
#include "model/ISortFilterListModel.hpp"
#include "model/ISpeakerListModel.hpp"
#include "model/ITrackListModel.hpp"

#include "model/FilterRoleModel.hpp"
#include "model/SortFilterProxyListModel.hpp"
#include "model/SortOrderModel.hpp"

namespace YADAW::Model
{
void initializeModel()
{
    QML_REGISTER_TYPE(IAssetDirectoryListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioBusChannelListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioBusConfigurationModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioDeviceIOGroupListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAudioDeviceListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IAutomationModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IFileTreeModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(ILocalizationListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IMixerChannelInsertListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IMixerChannelListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IParameterValueAndTextListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IPluginCategoryListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IPluginDirectoryListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IPluginListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(IPluginParameterListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(ISortFilterListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(ISpeakerListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(ITrackListModel, YADAW.Models, 1, 0);

    QML_REGISTER_TYPE(FilterRoleModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(SortFilterProxyListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(SortOrderModel, YADAW.Models, 1, 0);
}
}