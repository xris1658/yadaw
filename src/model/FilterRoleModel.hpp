#ifndef YADAW_SRC_MODEL_FILTERROLEMODEL
#define YADAW_SRC_MODEL_FILTERROLEMODEL

#include "model/IFilterRoleModel.hpp"
#include "model/ISortFilterListModel.hpp"
#include "util/OptionalUtil.hpp"

namespace YADAW::Model
{
class FilterRoleModel: public IFilterRoleModel
{
private:
    using DataType = std::vector<std::pair<int, Qt::CaseSensitivity>>;
public:
    FilterRoleModel(ISortFilterListModel* model, QObject* parent = nullptr);
    ~FilterRoleModel();
public:
    DataType::iterator begin() noexcept;
    DataType::const_iterator begin() const noexcept;
    DataType::iterator end() noexcept;
    DataType::const_iterator end() const noexcept;
    DataType::const_iterator cbegin() const noexcept;
    DataType::const_iterator cend() const noexcept;
public:
    int itemCount() const;
    OptionalRef<const std::pair<int, Qt::CaseSensitivity>> at(std::size_t index) const;
    OptionalRef<std::pair<int, Qt::CaseSensitivity>> at(std::size_t index);
    const std::pair<int, Qt::CaseSensitivity> operator[](std::size_t index) const;
    std::pair<int, Qt::CaseSensitivity> operator[](std::size_t index);
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    bool setFilterRole(int role, bool filter,
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive) override;
    void clear() override;
private:
    ISortFilterListModel* model_;
    std::vector<std::pair<int, Qt::CaseSensitivity>> filterRoles_;
};
}

#endif //YADAW_SRC_MODEL_FILTERROLEMODEL
