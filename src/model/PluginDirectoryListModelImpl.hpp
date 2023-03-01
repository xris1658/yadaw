#ifndef YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODELIMPL
#define YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODELIMPL

#include "model/PluginDirectoryListModel.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class PluginDirectoryListModelImpl: public PluginDirectoryListModel
{
Q_OBJECT
public:
    explicit PluginDirectoryListModelImpl(QObject* parent = nullptr);
    ~PluginDirectoryListModelImpl() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    Q_INVOKABLE void append(const QString& path);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void clear();
public:
    std::vector<QString> data_;
};
}

#endif //YADAW_CONTENT_MODEL_PLUGINDIRECTORYLISTMODELIMPL
