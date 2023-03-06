#ifndef YADAW_CONTENT_MODEL_PLUGINLISTMODEL
#define YADAW_CONTENT_MODEL_PLUGINLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class PluginListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Path,
        Uid,
        Name,
        Vendor,
        Version,
        Format,
        Type,
        RoleCount
    };
public:
    PluginListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~PluginListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual void clear() = 0;
    Q_INVOKABLE virtual void asyncUpdate() = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Id,      "plm_id"),
            std::make_pair(Role::Path,    "plm_path"),
            std::make_pair(Role::Uid,     "plm_uid"),
            std::make_pair(Role::Name,    "plm_name"),
            std::make_pair(Role::Vendor,  "plm_vendor"),
            std::make_pair(Role::Version, "plm_version"),
            std::make_pair(Role::Format,  "plm_format"),
            std::make_pair(Role::Type,    "plm_type")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_PLUGINLISTMODEL
