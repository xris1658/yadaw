#ifndef YADAW_SRC_MODEL_PLUGINLISTMODEL
#define YADAW_SRC_MODEL_PLUGINLISTMODEL

#include "dao/PluginTable.hpp"
#include "model/IPluginListModel.hpp"

#include <functional>

namespace YADAW::Model
{
class PluginListModel: public IPluginListModel
{
    Q_OBJECT
private:
    using List = std::vector<YADAW::DAO::PluginInfoInDatabase>;
public:
    PluginListModel(const std::function<List()>& updateListFunc, QObject* parent = nullptr);
    PluginListModel(std::function<List()>&& updateListFunc, QObject* parent = nullptr);
    ~PluginListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    Q_INVOKABLE void clear() override;
    Q_INVOKABLE void asyncUpdate() override;
public:
    void update();
private:
    std::function<List()> updateListFunc_;
    List data_;
};
}

#endif //YADAW_SRC_MODEL_PLUGINLISTMODEL
