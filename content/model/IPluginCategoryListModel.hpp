#ifndef YADAW_CONTENT_MODEL_IPLUGINCATEGORYLISTMODEL
#define YADAW_CONTENT_MODEL_IPLUGINCATEGORYLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>
#include <QString>

namespace YADAW::Model
{
class IPluginCategoryListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Name,
        RoleCount
    };
public:
    IPluginCategoryListModel(QObject* parent = nullptr):
        QAbstractListModel(parent)
    {}
    virtual ~IPluginCategoryListModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Id,   "pclm_id"),
            std::make_pair(Role::Name, "pclm_name")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IPLUGINCATEGORYLISTMODEL
