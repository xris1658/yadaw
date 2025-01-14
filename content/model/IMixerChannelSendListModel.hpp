#ifndef YADAW_CONTENT_MODEL_IMIXERCHANNELSENDLISTMODEL
#define YADAW_CONTENT_MODEL_IMIXERCHANNELSENDLISTMODEL

#include "ModelBase.hpp"
#include "entity/IAudioIOPosition.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IMixerChannelSendListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Destination = Qt::UserRole,
        IsPreFader,
        PolarityInverter, // YADAW::Model::IPolarityInverterModel
        Mute,
        Volume,
        EditingVolume,
        RoleCount
    };
    Q_ENUM(Role)
public:
    IMixerChannelSendListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IMixerChannelSendListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex& parent) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool append(bool isPreFader, YADAW::Entity::IAudioIOPosition* position) = 0;
    Q_INVOKABLE virtual bool remove(int position, int removeCount) = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Destination,      "mcsm_destination"),
            std::make_pair(IsPreFader,       "mcsm_is_pre_fader"),
            std::make_pair(PolarityInverter, "mcsm_polarity_inverter"),
            std::make_pair(Mute,             "mcsm_mute"),
            std::make_pair(Volume,           "mcsm_volume"),
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IMIXERCHANNELSENDLISTMODEL