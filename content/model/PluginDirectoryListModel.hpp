#ifndef YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODEL
#define YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODEL

#include "model/ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class PluginDirectoryListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Path = Qt::UserRole,
        RoleCount
    };
public:
    explicit PluginDirectoryListModel(QObject* parent = nullptr);
    ~PluginDirectoryListModel() override;
public:
    int itemCount() const;
    static constexpr int columnSize();
public:
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    Q_INVOKABLE void append(const QString& path);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void clear();
protected:
    RoleNames roleNames() const override;
public:
    std::vector<QString> data_;
};
}

#endif //YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODEL
