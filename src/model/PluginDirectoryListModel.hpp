#ifndef YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODEL
#define YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODEL

#include "model/IPluginDirectoryListModel.hpp"

#include <QAbstractListModel>
#include <QString>
#include <QUrl>

#include <vector>

namespace YADAW::Model
{
class PluginDirectoryListModel: public IPluginDirectoryListModel
{
Q_OBJECT
public:
    explicit PluginDirectoryListModel(QObject* parent = nullptr);
    ~PluginDirectoryListModel() override;
public:
    int itemCount() const;
    const QString& at(int i) const;
    const QString& operator[](int i) const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    Q_INVOKABLE void append(const QString& path) override;
    Q_INVOKABLE void append(const QUrl& url) override;
    Q_INVOKABLE void remove(int index) override;
public:
    std::vector<QString> data_;
};
}

#endif //YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODEL
