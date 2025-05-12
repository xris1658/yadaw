#ifndef YADAW_CONTENT_MODEL_ISPEAKERLISTMODEL
#define YADAW_CONTENT_MODEL_ISPEAKERLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class ISpeakerListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Type = Qt::UserRole,
        Name,
        RoleCount
    };
    enum SpeakerType
    {
        Custom,
        Invalid,
        Center,
        Left,
        Right,
        SideLeft,
        SideRight,
        RearLeft,
        RearRight,
        RearCenter,
        LFE
    };
    Q_ENUM(SpeakerType)
public:
    ISpeakerListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~ISpeakerListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Type, "slm_type"),
            std::make_pair(Role::Name, "slm_name")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_ISPEAKERLISTMODEL
