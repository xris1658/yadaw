#ifndef YADAW_CONTENT_MODEL_IPOLARITYINVERTERMODEL
#define YADAW_CONTENT_MODEL_IPOLARITYINVERTERMODEL

#include "ModelBase.hpp"

#include <QAbstractItemModel>

namespace YADAW::Model
{
class IPolarityInverterModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        ChannelName = Qt::UserRole,
        Inverted,
        RoleCount
    };
public:
    IPolarityInverterModel(QObject* parent = nullptr):
        QAbstractListModel(parent) {}
    virtual ~IPolarityInverterModel() override {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool toggle(int channelIndex) = 0;
    Q_INVOKABLE virtual void invertAll() = 0;
    Q_INVOKABLE virtual void revertAll() = 0;
    Q_INVOKABLE virtual void toggleAll() = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::ChannelName, "pim_channel_name"),
            std::make_pair(Role::Inverted, "pim_inverted")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IPOLARITYINVERTERMODEL