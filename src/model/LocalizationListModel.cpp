#include "LocalizationListModel.hpp"

#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
LocalizationListModel::LocalizationListModel(QObject* parent): ILocalizationListModel(parent)
{
    YADAW::Util::setCppOwnership(*this);
}

LocalizationListModel::~LocalizationListModel()
{

}

int LocalizationListModel::itemCount() const
{
    return data_.size();
}

const LocalizationListModel::Item& LocalizationListModel::at(int i) const
{
    return data_.at(i);
}

const LocalizationListModel::Item& LocalizationListModel::operator[](int i) const
{
    return data_[i];
}

int LocalizationListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant LocalizationListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
            return QVariant::fromValue(data_[row].name);
        case Role::Author:
            return QVariant::fromValue(data_[row].author);
        case Role::LanguageCode:
            return QVariant::fromValue(data_[row].languageCode);
        case Role::TranslationFileList:
            return QVariant::fromValue(data_[row].translationFileList);
        case Role::FontFamilyList:
            return QVariant::fromValue(data_[row].fontFamilyList);
        case Role::FontList:
            return QVariant::fromValue(data_[row].fontList);
        }
    }
    return {};
}

bool LocalizationListModel::append(const LocalizationListModel::Item& item)
{
    try
    {
        data_.emplace_back(item);
        return true;
    }
    catch(...)
    {
        return false;
    }
}

bool LocalizationListModel::remove(int index)
{
    if(index >= 0 && index < itemCount())
    {
        data_.erase(data_.begin() + index);
        return true;
    }
    return false;
}

void LocalizationListModel::clear()
{
    data_.clear();
}
}
