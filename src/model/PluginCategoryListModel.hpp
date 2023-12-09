#ifndef YADAW_SRC_MODEL_PLUGINCATEGORYLISTMODEL
#define YADAW_SRC_MODEL_PLUGINCATEGORYLISTMODEL

#include "dao/PluginCategoryTable.hpp"
#include "model/IPluginCategoryListModel.hpp"

namespace YADAW::Model
{
class PluginCategoryListModel: public IPluginCategoryListModel
{
    Q_OBJECT
private:
    using List = std::vector<YADAW::DAO::PluginCategoryInfo>;
public:
    PluginCategoryListModel(const std::function<List()>& updateListFunc, QObject* parent = nullptr);
    PluginCategoryListModel(std::function<List()>&& updateListFunc, QObject* parent = nullptr);
    ~PluginCategoryListModel() override;
public:
    int itemCount() const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    void update();
private:
    std::function<List()> updateListFunc_;
    List data_;
};
}

#endif //YADAW_SRC_MODEL_PLUGINCATEGORYLISTMODEL
