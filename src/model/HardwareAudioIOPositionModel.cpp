#include "HardwareAudioIOPositionModel.hpp"

namespace YADAW::Model
{
HardwareAudioIOPositionModel::HardwareAudioIOPositionModel(
    YADAW::Model::MixerChannelListModel& audioHardwareIOModel,
    QObject* parent):
    IAudioIOPositionModel(parent),
    audioHardwareIOModel_(&audioHardwareIOModel)
{
    auto count = audioHardwareIOModel.itemCount();
    positions_.reserve(count);
    std::generate_n(
        std::back_inserter(positions_), count,
        [this, i = 0U]() mutable
        {
            return std::unique_ptr<YADAW::Entity::HardwareAudioIOPosition>(
                new YADAW::Entity::HardwareAudioIOPosition(*this, i++)
            );
        }
    );
    QObject::connect(
        audioHardwareIOModel_, &YADAW::Model::MixerChannelListModel::rowsAboutToBeInserted,
        this, &HardwareAudioIOPositionModel::onSourceModelRowsAboutToBeInserted
    );
    QObject::connect(
        audioHardwareIOModel_, &YADAW::Model::MixerChannelListModel::rowsInserted,
        this, &HardwareAudioIOPositionModel::onSourceModelRowsInserted
    );
    QObject::connect(
        audioHardwareIOModel_, &YADAW::Model::MixerChannelListModel::rowsAboutToBeRemoved,
        this, &HardwareAudioIOPositionModel::onSourceModelRowsAboutToBeRemoved
    );
    QObject::connect(
        audioHardwareIOModel_, &YADAW::Model::MixerChannelListModel::rowsRemoved,
        this, &HardwareAudioIOPositionModel::onSourceModelRowsRemoved
    );
    QObject::connect(
        audioHardwareIOModel_, &YADAW::Model::MixerChannelListModel::dataChanged,
        this, &HardwareAudioIOPositionModel::onSourceModelDataChanged
    );
}

HardwareAudioIOPositionModel::~HardwareAudioIOPositionModel()
{}

int HardwareAudioIOPositionModel::itemCount() const
{
    return audioHardwareIOModel_->itemCount();
}

int HardwareAudioIOPositionModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant HardwareAudioIOPositionModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Position:
            return QVariant::fromValue<QObject*>(
                positions_[row].get()
            );
        case Role::ChannelConfig:
            return audioHardwareIOModel_->data(
                audioHardwareIOModel_->index(row),
                MixerChannelListModel::Role::ChannelConfig
            );
        case Role::ChannelCount:
            return audioHardwareIOModel_->data(
                audioHardwareIOModel_->index(row),
                MixerChannelListModel::Role::ChannelCount
            );
        }
    }
    return QVariant();
}

int HardwareAudioIOPositionModel::findIndexByID(const QString& id) const
{
    return audioHardwareIOModel_->getIndexOfId(id);
}

bool HardwareAudioIOPositionModel::isComparable(int roleIndex) const
{
    return roleIndex == IAudioIOPositionModel::Role::Position;
}

bool HardwareAudioIOPositionModel::isFilterable(int roleIndex) const
{
    return roleIndex == IAudioIOPositionModel::Role::ChannelConfig
        || roleIndex == IAudioIOPositionModel::Role::ChannelCount;
}

bool HardwareAudioIOPositionModel::isSearchable(int roleIndex) const
{
    return roleIndex == IAudioIOPositionModel::Role::Position;
}

bool HardwareAudioIOPositionModel::isLess(int roleIndex,
    const QModelIndex& lhs, const QModelIndex& rhs) const
{
    if(roleIndex == IAudioIOPositionModel::Role::Position)
    {
        return positions_[lhs.row()]->getName() < positions_[rhs.row()]->getName();
    }
    return false;
}

bool HardwareAudioIOPositionModel::isSearchPassed(int roleIndex,
    const QModelIndex& modelIndex, const QString& string,
    Qt::CaseSensitivity caseSensitivity) const
{
    if(roleIndex == IAudioIOPositionModel::Role::Position)
    {
        return positions_[modelIndex.row()]->getName().contains(
            string, caseSensitivity
        );
    }
    return true;
}

bool HardwareAudioIOPositionModel::isPassed(const QModelIndex& modelIndex,
    int role, const QVariant& variant) const
{
    if(isFilterable(role))
    {
        return data(modelIndex, role) == variant;
    }
    return true;
}

const YADAW::Model::MixerChannelListModel& HardwareAudioIOPositionModel::getModel() const
{
    return *audioHardwareIOModel_;
}

void HardwareAudioIOPositionModel::onSourceModelRowsAboutToBeInserted(
    const QModelIndex& parent, int start, int end)
{
    beginInsertRows(QModelIndex(), start, end);
}

void HardwareAudioIOPositionModel::onSourceModelRowsInserted(
    const QModelIndex& parent, int first, int last)
{
    std::generate_n(
        std::inserter(positions_, positions_.begin() + first),
        last - first + 1,
        [this, i = first]() mutable
        {
            return std::unique_ptr<YADAW::Entity::HardwareAudioIOPosition>(
                new YADAW::Entity::HardwareAudioIOPosition(*this, i++)
            );
        }
    );
    FOR_RANGE(i, last + 1, positions_.size())
    {
        positions_[i]->updateIndex(i);
    }
    endInsertRows();
}

void HardwareAudioIOPositionModel::onSourceModelRowsAboutToBeRemoved(
    const QModelIndex& parent, int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void HardwareAudioIOPositionModel::onSourceModelRowsRemoved(
    const QModelIndex& parent, int first, int last)
{
    positions_.erase(
        positions_.begin() + first,
        positions_.begin() + last + 1
    );
    FOR_RANGE(i, first, positions_.size())
    {
        positions_[i]->updateIndex(i);
    }
    endRemoveRows();
}

void HardwareAudioIOPositionModel::onSourceModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight,
    const QList<int>& roles)
{
    QList<int> destRoles;
    for(auto role: roles)
    {
        if(role == YADAW::Model::MixerChannelListModel::Role::Name)
        {
            destRoles.append(IAudioIOPositionModel::Role::Position);
        }
    }
    dataChanged(
        index(topLeft.row()), index(bottomRight.row()), destRoles
    );
}
}
