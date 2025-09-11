#ifndef YADAW_CONTENT_MODEL_IAUXIOTARGETLISTMODEL
#define YADAW_CONTENT_MODEL_IAUXIOTARGETLISTMODEL

#include "ModelBase.hpp"

#include "entity/IAudioIOPosition.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAuxIOTargetListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        ChannelIndex = Qt::UserRole,
        Target,
        RoleCount
    };
public:
    IAuxIOTargetListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAuxIOTargetListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::ChannelIndex, "aiotlm_channel_index"),
            std::make_pair(Role::Target,       "aiotlm_target")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IAUXIOTARGETLISTMODEL