#ifndef YADAW_SRC_MODEL_CONTEXTUSERDATA
#define YADAW_SRC_MODEL_CONTEXTUSERDATA

#include "controller/PluginContext.hpp"
#include "model/PluginParameterListModel.hpp"
#include "model/AudioDeviceIOGroupListModel.hpp"
#include "model/AudioDeviceAuxIOGroupListModel.hpp"
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
    std::unique_ptr<YADAW::Model::AudioDeviceAuxIOGroupListModel> audioAuxInputs;
    std::unique_ptr<YADAW::Model::AudioDeviceAuxIOGroupListModel> audioAuxOutputs;
    std::unique_ptr<YADAW::Model::AuxInputSourceListModel> audioAuxInputSources;
    std::unique_ptr<YADAW::Model::AuxOutputDestinationListModel> audioAuxOutputDestinations;
    QString name;
    PluginContextUserData(
        const YADAW::Controller::PluginContext& pluginContext,
        const QString& name
    );
    void initAuxModels(
        std::optional<std::uint32_t> mainAudioInput,
        std::optional<std::uint32_t> mainAudioOutput);
    const YADAW::Model::IAudioDeviceIOGroupListModel& getAudioAuxInputs() const;
    const YADAW::Model::IAudioDeviceIOGroupListModel& getAudioAuxOutputs() const;
    YADAW::Model::IAudioDeviceIOGroupListModel& getAudioAuxInputs();
    YADAW::Model::IAudioDeviceIOGroupListModel& getAudioAuxOutputs();
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