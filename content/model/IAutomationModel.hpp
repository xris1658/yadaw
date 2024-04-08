#ifndef YADAW_CONTENT_MODEL_IAUTOMATIONMODEL
#define YADAW_CONTENT_MODEL_IAUTOMATIONMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAutomationModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(double minValue)
    Q_PROPERTY(double maxValue)
public:
    enum Role
    {
        Time = Qt::UserRole,
        Value,
        Curve,
        RoleCount
    };
public:
    IAutomationModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAutomationModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    virtual double minValue() const = 0;
    virtual double maxValue() const = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Time,  "am_time"),
            std::make_pair(Role::Value, "am_value"),
            std::make_pair(Role::Curve, "am_curve")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IAUTOMATIONMODEL
