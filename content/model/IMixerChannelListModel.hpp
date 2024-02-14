#ifndef YADAW_CONTENT_MODEL_IMIXERCHANNELLISTMODEL
#define YADAW_CONTENT_MODEL_IMIXERCHANNELLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IMixerChannelListModel: public QAbstractListModel
{
    Q_OBJECT
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
        Name = Qt::UserRole,
        Color,
        ChannelType,
        InputExist,
        Input,
        InputType,
        OutputExist,
        Output,
        OutputType,
        InstrumentExist,
        Instrument,
        InsertExist,
        Inserts,
        SendExist,
        Sends,
        Mute,
        Solo,
        InvertPolarityExist,
        InvertPolarity,
        ArmRecordingExist,
        ArmRecording,
        Volume,
        Panning,
        DBMeter,
        RoleCount
    };
public:
    IMixerChannelListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IMixerChannelListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool insert(int position, ChannelTypes type) = 0;
    Q_INVOKABLE virtual bool append(ChannelTypes type) = 0;
    Q_INVOKABLE virtual bool remove(int position, int removeCount) = 0;
    Q_INVOKABLE virtual bool move(int position, int moveCount, int newPosition) = 0;
    Q_INVOKABLE virtual bool copy(int position, int copyCount, int newPosition) = 0;
    Q_INVOKABLE virtual void clear() = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Name,                "mclm_name"),
            std::make_pair(Color,               "mclm_color"),
            std::make_pair(ChannelType,         "mclm_channel_type"),
            std::make_pair(InputExist,          "mclm_input_exist"),
            std::make_pair(Input,               "mclm_input"),
            std::make_pair(InputType,           "mclm_input_type"),
            std::make_pair(OutputExist,         "mclm_output_exist"),
            std::make_pair(Output,              "mclm_output"),
            std::make_pair(OutputType,          "mclm_output_type"),
            std::make_pair(InstrumentExist,     "mclm_instrument_exist"),
            std::make_pair(Instrument,          "mclm_instrument"),
            std::make_pair(InsertExist,         "mclm_insert_exist"),
            std::make_pair(Inserts,             "mclm_inserts"),
            std::make_pair(SendExist,           "mclm_send_exist"),
            std::make_pair(Sends,               "mclm_sends"),
            std::make_pair(Mute,                "mclm_mute"),
            std::make_pair(Solo,                "mclm_solo"),
            std::make_pair(InvertPolarityExist, "mclm_invert_polarity_exist"),
            std::make_pair(InvertPolarity,      "mclm_invert_polarity"),
            std::make_pair(ArmRecordingExist,   "mclm_arm_recording_exist"),
            std::make_pair(ArmRecording,        "mclm_arm_recording"),
            std::make_pair(Volume,              "mclm_volume"),
            std::make_pair(Panning,             "mclm_panning"),
            std::make_pair(DBMeter,             "mclm_db_meter")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IMIXERCHANNELLISTMODEL
