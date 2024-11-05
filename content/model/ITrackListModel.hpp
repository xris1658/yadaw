#ifndef YADAW_CONTENT_MODEL_ITRACKLISTMODEL
#define YADAW_CONTENT_MODEL_ITRACKLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>
#include <QString>

namespace YADAW::Model
{
class ITrackListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Name = Qt::UserRole,
        Type,
        Color,
        Height,
        Contents,
        Automations,
        Input,
        Output,
        Instrument,
        Inserts,
        Sends,
        Mute,
        Solo,
        InvertPolarity,
        ArmRecording,
        Volume,
        Panning,
        Mono,
        RoleCount
    };
public:
    enum TrackType
    {
        Audio,
        MIDI,
        HardwareInput,
        HardwareOutput,
        AudioFX,
        Group
    };
    Q_ENUM(TrackType)
public:
    ITrackListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~ITrackListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual void insert(int index, int trackType, int count) = 0;
    Q_INVOKABLE virtual void append(int count, int trackType) = 0;
    Q_INVOKABLE virtual void removeTrack(int index) = 0;
    Q_INVOKABLE virtual void removeTracks(int from, int to) = 0;
    Q_INVOKABLE virtual void moveTrack(int index, int newIndex) = 0;
    Q_INVOKABLE virtual void moveTracks(int from, int to, int newIndex) = 0;
    Q_INVOKABLE virtual void copyTrack(int index, int toIndex) = 0;
    Q_INVOKABLE virtual void copyTracks(int from, int to, int toIndex) = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Name,           "tlm_name"),
            std::make_pair(Role::Type,           "tlm_type"),
            std::make_pair(Role::Color,          "tlm_color"),
            std::make_pair(Role::Height,         "tlm_height"),
            std::make_pair(Role::Contents,       "tlm_contents"),
            std::make_pair(Role::Automations,    "tlm_automations"),
            std::make_pair(Role::Input,          "tlm_input"),
            std::make_pair(Role::Output,         "tlm_output"),
            std::make_pair(Role::Instrument,     "tlm_instrument"),
            std::make_pair(Role::Inserts,        "tlm_inserts"),
            std::make_pair(Role::Sends,          "tlm_sends"),
            std::make_pair(Role::Mute,           "tlm_mute"),
            std::make_pair(Role::Solo,           "tlm_solo"),
            std::make_pair(Role::InvertPolarity, "tlm_invert_polarity"),
            std::make_pair(Role::ArmRecording,   "tlm_arm_recording"),
            std::make_pair(Role::Volume,         "tlm_volume"),
            std::make_pair(Role::Panning,        "tlm_panning"),
            std::make_pair(Role::Mono,           "tlm_mono")
        };
        return ret;
    }
};
}
#endif // YADAW_CONTENT_MODEL_ITRACKLISTMODEL
