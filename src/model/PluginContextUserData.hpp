#ifndef YADAW_SRC_MODEL_CONTEXTUSERDATA
#define YADAW_SRC_MODEL_CONTEXTUSERDATA

#include "controller/PluginContext.hpp"
#include "model/PluginParameterListModel.hpp"
#include "model/AudioDeviceIOGroupListModel.hpp"

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
    QString name;
    PluginContextUserData(
        const YADAW::Controller::PluginContext& pluginContext,
        const QString& name
    );
};
}

#endif // YADAW_SRC_MODEL_CONTEXTUSERDATA