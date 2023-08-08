#include "AutomationModel.hpp"

namespace YADAW::Model
{
AutomationModel::AutomationModel(Audio::Base::Automation& automation, QObject* parent):
    YADAW::Model::IAutomationModel(parent), automation_(&automation)
{}

AutomationModel::~AutomationModel()
{}

std::size_t AutomationModel::itemCount() const
{
    return automation_->pointCount();
}

double AutomationModel::getMinValue() const
{
    return automation_->minValue();
}

double AutomationModel::getMaxValue() const
{
    return automation_->maxValue();
}

int AutomationModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant AutomationModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        const auto& point = automation_->operator[](row);
        switch(role)
        {
        case Role::Time:
            return QVariant::fromValue<int>(point.time());
        case Role::Value:
            return QVariant::fromValue(point.value());
        case Role::Curve:
            return QVariant::fromValue(point.curve());
        }
    }
    return {};
}

bool AutomationModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto& point = automation_->operator[](row);
        switch(role)
        {
        case Role::Value:
            point.setValue(value.value<double>());
            return true;
        case Role::Curve:
            point.setCurve(value.value<double>());
            return true;
        }
    }
    return false;
}
}