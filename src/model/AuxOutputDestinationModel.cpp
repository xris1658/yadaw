#include "AuxOutputDestinationModel.hpp"

namespace YADAW::Model
{
AuxOutputDestinationModel::AuxOutputDestinationModel(
    YADAW::Model::AuxOutputDestinationListModel& model,
    std::uint32_t row, QObject* parent):
    IAuxOutputDestinationListModel(parent),
    model_(&model), row_(row)
{
}

AuxOutputDestinationModel::~AuxOutputDestinationModel()
{
}

int AuxOutputDestinationModel::rowCount(const QModelIndex& parent) const
{
    return positions_.size();
}

QVariant AuxOutputDestinationModel::data(const QModelIndex& index, int role) const
{
    return {}; // TODO
}

bool AuxOutputDestinationModel::append(YADAW::Entity::IAudioIOPosition* position)
{
    return false; // TODO
}

bool AuxOutputDestinationModel::remove(int position, int removeCount)
{
    return false; // TODO
}
}
