#ifndef YADAW_SRC_MODEL_ASSETDIRECTORYLISTMODEL
#define YADAW_SRC_MODEL_ASSETDIRECTORYLISTMODEL

#include "model/IAssetDirectoryListModel.hpp"
#include "model/FileTreeModel.hpp"

#include <QString>
#include <QUrl>

#include <vector>

namespace YADAW::Model
{
class AssetDirectoryListModel: public IAssetDirectoryListModel
{
    Q_OBJECT
private:
    struct Data
    {
        Data(const QString& path, const QString& name,
            std::unique_ptr<YADAW::Model::FileTreeModel>&& fileTreeModel, int id):
            path(path), name(name), fileTreeModel(std::move(fileTreeModel)), id(id)
        {}
        QString path;
        QString name;
        std::unique_ptr<YADAW::Model::FileTreeModel> fileTreeModel;
        int id;
    };
public:
    AssetDirectoryListModel(QObject* parent = nullptr);
    ~AssetDirectoryListModel() override;
public:
    std::uint32_t itemCount() const;
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
    std::vector<Data> data_;
};
}

#endif // YADAW_SRC_MODEL_ASSETDIRECTORYLISTMODEL
