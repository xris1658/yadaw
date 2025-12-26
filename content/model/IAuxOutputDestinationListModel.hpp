#ifndef YADAW_CONTENT_MODEL_IAUXOUTPUTDESTINATIONLISTMODEL
#define YADAW_CONTENT_MODEL_IAUXOUTPUTDESTINATIONLISTMODEL

#include "ModelBase.hpp"
#include "entity/IAudioIOPosition.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAuxOutputDestinationListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Destination = Qt::UserRole,
        RoleCount
    };
public:
    IAuxOutputDestinationListModel(QObject *parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAuxOutputDestinationListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool append(YADAW::Entity::IAudioIOPosition* position) = 0;
    Q_INVOKABLE virtual bool remove(int position, int removeCount = 1) = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Destination, "aodlm_destination")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IAUXOUTPUTDESTINATIONLISTMODEL