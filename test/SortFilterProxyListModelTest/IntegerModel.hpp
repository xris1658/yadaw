#ifndef YADAW_TEST_SORTFILTERPROXYLISTMODELTEST_INTEGERMODEL
#define YADAW_TEST_SORTFILTERPROXYLISTMODELTEST_INTEGERMODEL

#include "model/ISortFilterListModel.hpp"

class IntegerModel: public YADAW::Model::ISortFilterListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Value
    };
    Q_ENUM(Role)
public:
    IntegerModel(QObject* parent = nullptr);
    ~IntegerModel() override;
public:
    int itemCount() const;
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
    bool isComparable(int roleIndex) const override;
    bool isFilterable(int roleIndex) const override;
    bool isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const override;
    bool isPassed(int roleIndex, const QModelIndex& modelIndex, const QString& string,
        Qt::CaseSensitivity caseSensitivity) const override;
public:
    bool insert(int position, const QList<int>& data);
    bool append(const QList<int>& data);
    bool remove(int position, int count);
    void clear();
protected:
    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> ret
        {
            std::make_pair(Role::Value, "im_value")
        };
        return ret;
    }
private:
    std::vector<int> data_;
};

#endif //YADAW_TEST_SORTFILTERPROXYLISTMODELTEST_INTEGERMODEL