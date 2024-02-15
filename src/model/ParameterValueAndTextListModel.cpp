#include "ParameterValueAndTextListModel.hpp"

#include "model/PluginParameterListModel.hpp"

namespace YADAW::Model
{
ParameterValueAndTextListModel::ParameterValueAndTextListModel(
    PluginParameterListModel& model,
    int index,
    QObject* parent):
    IParameterValueAndTextListModel(parent),
    model_(&model),
    index_(index)
{}

ParameterValueAndTextListModel::~ParameterValueAndTextListModel()
{}

int ParameterValueAndTextListModel::itemCount() const
{
    return model_->parameter().parameter(index_)->stepCount() + 1;
}

int ParameterValueAndTextListModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant ParameterValueAndTextListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& parameter = *(model_->parameter().parameter(index_));
        auto minValue = parameter.minValue();
        auto maxValue = parameter.maxValue();
        auto stepCount = parameter.stepCount();
        auto value = minValue + (maxValue - minValue) / stepCount;
        switch(role)
        {
        case Role::Value:
        {
            return QVariant::fromValue<double>(value);
        }
        case Role::Text:
        {
            return QVariant::fromValue<QString>(parameter.valueToString(value));
        }
        }
    }
    return QVariant();
}
}