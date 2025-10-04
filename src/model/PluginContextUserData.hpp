#ifndef YADAW_SRC_MODEL_CONTEXTUSERDATA
#define YADAW_SRC_MODEL_CONTEXTUSERDATA

#include "controller/PluginContext.hpp"
#include "model/PluginParameterListModel.hpp"
#include "model/AudioDeviceIOGroupListModel.hpp"
#include "model/AuxInputSourceListModel.hpp"
#include "model/AuxOutputDestinationListModel.hpp"

#include <QMetaObject>
#include <QString>

namespace YADAW::Model
{
struct PluginContextUserData
{
    QMetaObject::Connection pluginWindowConnection;
    QMetaObject::Connection genericEditorWindowConnection;
    YADAW::Model::PluginParameterListModel paramListModel;
    YADAW::Model::AudioDeviceIOGroupListModel audioInputs;
    YADAW::Model::AudioDeviceIOGroupListModel audioOutputs;
    std::unique_ptr<YADAW::Model::AuxInputSourceListModel> audioAuxInputSources;
    std::unique_ptr<YADAW::Model::AuxOutputDestinationListModel> audioAuxOutputDestinations;
    QString name;
    PluginContextUserData(
        const YADAW::Controller::PluginContext& pluginContext,
        const QString& name
    );
};

using FillPluginContextCallback = bool(
    YADAW::Controller::PluginContext&,
    const YADAW::Controller::InitPluginArgs& initPluginArgs
);

namespace Impl
{
bool blankFillPluginContext(
    YADAW::Controller::PluginContext& context,
    const YADAW::Controller::InitPluginArgs& initPluginArgs);
}
}

#endif // YADAW_SRC_MODEL_CONTEXTUSERDATA