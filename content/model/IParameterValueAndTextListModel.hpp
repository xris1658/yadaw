#ifndef YADAW_CONTENT_MODEL_IPARAMETERVALUEANDTEXTLISTMODEL
#define YADAW_CONTENT_MODEL_IPARAMETERVALUEANDTEXTLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IParameterValueAndTextListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Value = Qt::UserRole,
        Text,
        RoleCount
    };
public:
    IParameterValueAndTextListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IParameterValueAndTextListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Value, "pvatlm_value"),
            std::make_pair(Role::Text,  "pvatlm_text")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IPARAMETERVALUEANDTEXTLISTMODEL
