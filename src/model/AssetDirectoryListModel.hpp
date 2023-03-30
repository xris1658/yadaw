#ifndef YADAW_SRC_MODEL_ASSETDIRECTORYLISTMODEL
#define YADAW_SRC_MODEL_ASSETDIRECTORYLISTMODEL

#include "model/IAssetDirectoryListModel.hpp"

#include <QString>
#include <QUrl>

#include <vector>

namespace YADAW::Model
{
class AssetDirectoryListModel: public IAssetDirectoryListModel
{
    Q_OBJECT
public:
    AssetDirectoryListModel(QObject* parent = nullptr);
    ~AssetDirectoryListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    Q_INVOKABLE void append(const QString& path, const QString& name) override;
    Q_INVOKABLE void append(const QUrl& url) override;
    Q_INVOKABLE void rename(int id, const QString& name) override;
    Q_INVOKABLE void remove(int id) override;
private:
    std::vector<std::tuple<int, QString, QString>> data_;
};
}

#endif //YADAW_SRC_MODEL_ASSETDIRECTORYLISTMODEL
