#include "AutomationModel.hpp"

namespace YADAW::Model
{
AutomationModel::AutomationModel(Audio::Base::Automation& automation, QObject* parent):
    YADAW::Model::IAutomationModel(parent), automation_(&automation)
{}

AutomationModel::~AutomationModel()
{}

std::size_t AutomationModel::pointCount() const
{
    return automation_->pointCount();
}

int AutomationModel::rowCount(const QModelIndex&) const
{
    return pointCount();
}

QVariant AutomationModel::data(const QModelIndex& index, int role) const
{
    auto automation = *automation_;
    auto row = index.row();
    if(row < pointCount() && row >= 0)
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
}