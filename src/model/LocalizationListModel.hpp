#ifndef YADAW_SRC_MODEL_LOCALIZATIONLISTMODEL
#define YADAW_SRC_MODEL_LOCALIZATIONLISTMODEL

#include "model/ILocalizationListModel.hpp"

#include <QString>
#include <vector>

namespace YADAW::Model
{
class LocalizationListModel: public ILocalizationListModel
{
public:
    struct Item
    {
        QString name;
        QString author;
        QString languageCode;
        QString translationFileName;
        std::vector<QString> fontFamilyList;
        std::vector<QString> fontList;
    };
public:
    explicit LocalizationListModel(QObject* parent = nullptr);
    ~LocalizationListModel() override;
public:
    int itemCount() const;
    const Item& at(int i) const;
    const Item& operator[](int i) const;
public:
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    bool append(const Item& item);
    bool remove(int index);
    void clear();
private:
    std::vector<Item> data_;
};
}

#endif // YADAW_SRC_MODEL_LOCALIZATIONLISTMODEL
