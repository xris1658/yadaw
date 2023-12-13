#ifndef YADAW_SRC_MODEL_SORTORDERMODEL
#define YADAW_SRC_MODEL_SORTORDERMODEL

#include "model/ISortOrderModel.hpp"
#include "model/IComparableListModel.hpp"
#include "util/OptionalUtil.hpp"

#include <vector>

namespace YADAW::Model
{
class SortOrderModel: public ISortOrderModel
{
public:
    SortOrderModel(IComparableListModel* model, QObject* parent = nullptr);
    ~SortOrderModel();
public:
    int itemCount() const;
    OptionalRef<const std::pair<int, Qt::SortOrder>> at(std::size_t index) const;
    OptionalRef<std::pair<int, Qt::SortOrder>> at(std::size_t index);
    const std::pair<int, Qt::SortOrder>& operator[](std::size_t index) const;
    std::pair<int, Qt::SortOrder>& operator[](std::size_t index);
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    Q_INVOKABLE bool insert(int role, Qt::SortOrder sortOrder, int position) override;
    Q_INVOKABLE bool append(int role, Qt::SortOrder sortOrder) override;
    Q_INVOKABLE int getIndexOfRole(int role) const override;
    Q_INVOKABLE bool remove(int index) override;
private:
    IComparableListModel* model_;
    std::vector<std::pair<int, Qt::SortOrder>> sortOrder_;
};
}

#endif //YADAW_SRC_MODEL_SORTORDERMODEL
