#ifndef YADAW_CONTENT_MODEL_ASSETDIRECTORYLISTMODEL
#define YADAW_CONTENT_MODEL_ASSETDIRECTORYLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

#include <utility>

namespace YADAW::Model
{
class AssetDirectoryListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Id = Qt::UserRole,
        Path,
        Name,
        RoleCount
    };
public:
    AssetDirectoryListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~AssetDirectoryListModel() {}
public:
    static constexpr int columnCount() { return RoleCount - Qt::UserRole; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual void append(int id, const QString& path, const QString& name) = 0;
    Q_INVOKABLE virtual void remove(int id) = 0;
    Q_INVOKABLE virtual void clear() = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Id, "pathId"),
            std::make_pair(Role::Path, "path"),
            std::make_pair(Role::Name, "name")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_ASSETDIRECTORYLISTMODEL
