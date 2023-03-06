#ifndef YADAW_SRC_MODEL_PLUGINLISTMODELIMPL
#define YADAW_SRC_MODEL_PLUGINLISTMODELIMPL

#include "dao/PluginTable.hpp"
#include "model/PluginListModel.hpp"

#include <functional>

namespace YADAW::Model
{
class PluginListModelImpl: public PluginListModel
{
    Q_OBJECT
private:
    using List = std::vector<YADAW::DAO::PluginInfoInDatabase>;
public:
    PluginListModelImpl(const std::function<List()>& updateListFunc, QObject* parent = nullptr);
    PluginListModelImpl(std::function<List()>&& updateListFunc, QObject* parent = nullptr);
    ~PluginListModelImpl() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    Q_INVOKABLE void clear() override;
    Q_INVOKABLE void asyncUpdate() override;
private:
    void update();
private:
    std::function<List()> updateListFunc_;
    List data_;
};
}

#endif //YADAW_SRC_MODEL_PLUGINLISTMODELIMPL
