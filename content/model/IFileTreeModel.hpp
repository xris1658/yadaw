#ifndef YADAW_CONTENT_MODEL_IFILETREEMODEL
#define YADAW_CONTENT_MODEL_IFILETREEMODEL

#include "ModelBase.hpp"

#include <QAbstractItemModel>

namespace YADAW::Model
{
class IFileTreeModel: public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Role
    {
        Name = Qt::UserRole,
        Path,
        IsDirectory,
        RoleCount
    };
public:
    IFileTreeModel(QObject* parent = nullptr): QAbstractItemModel(parent) {}
    virtual ~IFileTreeModel() {}
public:
    static constexpr int roleCount() { return YADAW::Util::underlyingValue(RoleCount) - YADAW::Util::underlyingValue(Qt::UserRole); }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Name,        "ftm_name"),
            std::make_pair(Role::Path,        "ftm_path"),
            std::make_pair(Role::IsDirectory, "ftm_is_directory")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IFILETREEMODEL
