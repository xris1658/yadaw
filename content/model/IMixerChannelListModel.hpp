#ifndef YADAW_CONTENT_MODEL_IMIXERCHANNELLISTMODEL
#define YADAW_CONTENT_MODEL_IMIXERCHANNELLISTMODEL

#include "ModelBase.hpp"

#include "entity/ChannelConfig.hpp"
#include "model/ISortFilterListModel.hpp"

namespace YADAW::Model
{
class IMixerChannelListModel: public ISortFilterListModel
{
    Q_OBJECT
    Q_PROPERTY(bool hasMute READ hasMute NOTIFY hasMuteChanged)
public:
    enum ChannelTypes
    {
        ChannelTypeAudio,
        ChannelTypeInstrument,
        ChannelTypeAudioFX,
        ChannelTypeBus,
        ChannelTypeAudioHardwareInput,
        ChannelTypeAudioHardwareOutput
    };
    Q_ENUM(ChannelTypes)
    enum MediaTypes
    {
        MediaTypeAudio,
        MediaTypeMIDI
    };
    Q_ENUM(MediaTypes)
public:
    enum Role
    {
        Id = Qt::UserRole, // QString::number(std::uint64_t)
        Name,
        NameWithIndex,
        Color,
        ChannelType,
        ChannelConfig, // YADAW::Entity::ChannelConfig::Config
        ChannelCount,
        InputExist,
        Input, // YADAW::Entity::AudioIOPosition
        InputSource, // YADAW::Entity::AudioIOPosition
        InputType, // MediaTypes
        OutputExist,
        Output, // YADAW::Entity::AudioIOPosition
        OutputDestination, // YADAW::Entity::AudioIOPosition
        OutputType, // MediaTypes
        InstrumentExist,
        InstrumentBypassed,
        InstrumentName,
        InstrumentAudioInputs,
        InstrumentAudioOutputs,
        InstrumentAudioAuxInputSource,
        InstrumentAudioAuxOutputDestination,
        InstrumentEventInputs,
        InstrumentEventOutputs,
        InstrumentHasUI,
        InstrumentWindowVisible,
        InstrumentGenericEditorVisible,
        InstrumentLatency,
        Inserts,
        SendExist,
        Sends,
        Mute,
        Solo,
        PolarityInverter, // YADAW::Model::IPolarityInverterModel
        MonitorExist,
        Monitor,
        ArmRecordingExist,
        ArmRecording,
        Volume,
        Panning,
        DBMeter,
        EditingVolume,
        RoleCount
    };
    Q_ENUM(Role)
public:
    IMixerChannelListModel(QObject* parent = nullptr): ISortFilterListModel(parent) {}
    virtual ~IMixerChannelListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool insert(int position, int count, ChannelTypes type,
        YADAW::Entity::ChannelConfig::Config channelConfig,
        int channelCount = 0) = 0;
    Q_INVOKABLE virtual bool append(int count, ChannelTypes type,
        YADAW::Entity::ChannelConfig::Config channelConfig,
        int channelCount = 0) = 0;
    Q_INVOKABLE virtual bool remove(int position, int removeCount) = 0;
    Q_INVOKABLE virtual bool move(int position, int moveCount, int newPosition) = 0;
    Q_INVOKABLE virtual bool copy(int position, int copyCount, int newPosition) = 0;
    Q_INVOKABLE virtual void clear() = 0;
    Q_INVOKABLE virtual bool setInstrument(int position, int pluginId) = 0;
    Q_INVOKABLE virtual bool removeInstrument(int position) = 0;
    Q_INVOKABLE virtual int getIndexOfId(const QString& id) const = 0;
    Q_INVOKABLE virtual bool hasMute() const = 0;
    Q_INVOKABLE virtual void unmuteAll() = 0;
signals:
    void hasMuteChanged();
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Id,                                  "mclm_id"),
            std::make_pair(Name,                                "mclm_name"),
            std::make_pair(NameWithIndex,                       "mclm_name_with_index"),
            std::make_pair(Color,                               "mclm_color"),
            std::make_pair(ChannelType,                         "mclm_channel_type"),
            std::make_pair(ChannelConfig,                       "mclm_channel_config"),
            std::make_pair(ChannelCount,                        "mclm_channel_count"),
            std::make_pair(InputExist,                          "mclm_input_exist"),
            std::make_pair(Input,                               "mclm_input"),
            std::make_pair(InputSource,                         "mclm_input_source"),
            std::make_pair(InputType,                           "mclm_input_type"),
            std::make_pair(OutputExist,                         "mclm_output_exist"),
            std::make_pair(Output,                              "mclm_output"),
            std::make_pair(OutputDestination,                   "mclm_output_destination"),
            std::make_pair(OutputType,                          "mclm_output_type"),
            std::make_pair(InstrumentExist,                     "mclm_instrument_exist"),
            std::make_pair(InstrumentBypassed,                  "mclm_instrument_bypassed"),
            std::make_pair(InstrumentName,                      "mclm_instrument_name"),
            std::make_pair(InstrumentAudioInputs,               "mclm_instrument_audio_inputs"),
            std::make_pair(InstrumentAudioOutputs,              "mclm_instrument_audio_outputs"),
            std::make_pair(InstrumentAudioAuxInputSource,       "mclm_instrument_audio_aux_input_source"),
            std::make_pair(InstrumentAudioAuxOutputDestination, "mclm_instrument_audio_aux_output_destination"),
            std::make_pair(InstrumentEventInputs,               "mclm_instrument_event_inputs"),
            std::make_pair(InstrumentEventOutputs,              "mclm_instrument_event_outputs"),
            std::make_pair(InstrumentHasUI,                     "mclm_instrument_has_ui"),
            std::make_pair(InstrumentWindowVisible,             "mclm_instrument_window_visible"),
            std::make_pair(InstrumentGenericEditorVisible,      "mclm_instrument_generic_editor_visible"),
            std::make_pair(InstrumentLatency,                   "mclm_instrument_latency"),
            std::make_pair(Inserts,                             "mclm_inserts"),
            std::make_pair(SendExist,                           "mclm_send_exist"),
            std::make_pair(Sends,                               "mclm_sends"),
            std::make_pair(Mute,                                "mclm_mute"),
            std::make_pair(Solo,                                "mclm_solo"),
            std::make_pair(MonitorExist,                        "mclm_monitor_exist"),
            std::make_pair(Monitor,                             "mclm_monitor"),
            std::make_pair(PolarityInverter,                    "mclm_polarity_inverter"),
            std::make_pair(ArmRecordingExist,                   "mclm_arm_recording_exist"),
            std::make_pair(ArmRecording,                        "mclm_arm_recording"),
            std::make_pair(Volume,                              "mclm_volume"),
            std::make_pair(Panning,                             "mclm_panning"),
            std::make_pair(DBMeter,                             "mclm_db_meter"),
            std::make_pair(EditingVolume,                       "mclm_editing_volume")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IMIXERCHANNELLISTMODEL
