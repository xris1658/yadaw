#include "ModelInitializer.hpp"

#include "util/QmlUtil.hpp"
#include "model/IAssetDirectoryListModel.hpp"
#include "model/IAudioBusChannelListModel.hpp"
#include "model/IAudioBusConfigurationModel.hpp"
#include "model/IAudioDeviceIOGroupListModel.hpp"
#include "model/IAudioDeviceListModel.hpp"
#include "model/IAudioIOPositionItemModel.hpp"
#include "model/IAudioIOPositionModel.hpp"
#include "model/IAutomationModel.hpp"
#include "model/IFileTreeModel.hpp"
#include "model/ILocalizationListModel.hpp"
#include "model/IMixerChannelInsertListModel.hpp"
#include "model/IMixerChannelListModel.hpp"
#include "model/IMixerChannelSendListModel.hpp"
#include "model/INativePopupEventFilterModel.hpp"
#include "model/IParameterValueAndTextListModel.hpp"
#include "model/IPluginCategoryListModel.hpp"
#include "model/IPluginDirectoryListModel.hpp"
#include "model/IPluginListModel.hpp"
#include "model/IPluginParameterListModel.hpp"
#include "model/IPolarityInverterModel.hpp"
#include "model/ISortFilterItemModel.hpp"
#include "model/ISortFilterListModel.hpp"
#include "model/ISpeakerListModel.hpp"
#include "model/ITrackListModel.hpp"

#include "model/FilterRoleModel.hpp"
#include "model/MessageDialogButtonBoxModel.hpp"
#include "model/SortFilterProxyListModel.hpp"
#include "model/SortOrderModel.hpp"

namespace YADAW::Model
{
QString modelIsInterfaceText = "This model is an abstract model instantiated in C++ codes.";

void initializeModel()
{
    QML_REGISTER_UNCREATABLE_TYPE(IAssetDirectoryListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IAudioBusChannelListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IAudioBusConfigurationModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IAudioDeviceIOGroupListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IAudioDeviceListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IAudioIOPositionItemModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IAudioIOPositionModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IAutomationModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IFileTreeModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(ILocalizationListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IMixerChannelInsertListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IMixerChannelListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IMixerChannelSendListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(INativePopupEventFilterModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IParameterValueAndTextListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IPluginCategoryListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IPluginDirectoryListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IPluginListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IPluginParameterListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(IPolarityInverterModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(ISortFilterItemModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(ISortFilterListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(ISpeakerListModel, YADAW.Models, 1, 0, modelIsInterfaceText);
    QML_REGISTER_UNCREATABLE_TYPE(ITrackListModel, YADAW.Models, 1, 0, modelIsInterfaceText);

    QML_REGISTER_TYPE(FilterRoleModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(MessageDialogButtonBoxModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(SortFilterProxyListModel, YADAW.Models, 1, 0);
    QML_REGISTER_TYPE(SortOrderModel, YADAW.Models, 1, 0);
}
}