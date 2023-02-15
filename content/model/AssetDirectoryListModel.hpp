#ifndef YADAW_CONTENT_MODEL_ASSETDIRECTORYLISTMODEL
#define YADAW_CONTENT_MODEL_ASSETDIRECTORYLISTMODEL

#include "model/ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class AssetDirectoryListModel: public QAbstractListModel
{
public:
    enum Role
    {
        Id = Qt::UserRole,
        Path,
        Name,
        RoleCount
    };
public:
    explicit AssetDirectoryListModel(QObject* parent = nullptr);
    ~AssetDirectoryListModel() override;
public:
    int itemCount() const;
    static constexpr int columnSize();
public:
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    Q_INVOKABLE void append(int id, const QString& path, const QString& name);
    Q_INVOKABLE void remove(int id);
    Q_INVOKABLE void clear();
protected:
    RoleNames roleNames() const override;
private:
    std::vector<std::tuple<int, QString, QString>> data_;
};
}

#endif //YADAW_CONTENT_MODEL_ASSETDIRECTORYLISTMODEL
