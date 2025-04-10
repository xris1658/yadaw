#include "RegularAudioIOPositionModel.hpp"

#include "model/IMixerChannelListModel.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
RegularAudioIOPositionModel::RegularAudioIOPositionModel(
    YADAW::Model::MixerChannelListModel& model,
    YADAW::Model::IMixerChannelListModel::ChannelTypes type, QObject* parent):
    IAudioIOPositionModel(parent),
    model_(model)
{
    YADAW::Util::setCppOwnership(*this);
    auto count = model.itemCount();
    positions_.reserve(count);
    std::generate_n(
        std::back_inserter(positions_), count,
        [this, i = 0U]() mutable
        {
            return std::unique_ptr<YADAW::Entity::RegularAudioIOPosition>(
                new YADAW::Entity::RegularAudioIOPosition(*this, i++)
            );
        }
    );
    model_.setValueOfFilter(
        IMixerChannelListModel::Role::ChannelType, type
        );
    QObject::connect(
        &model_, &YADAW::Model::SortFilterProxyListModel::rowsAboutToBeInserted,
        this, &RegularAudioIOPositionModel::onSourceModelRowsAboutToBeInserted
    );
    QObject::connect(
        &model_, &YADAW::Model::SortFilterProxyListModel::rowsInserted,
        this, &RegularAudioIOPositionModel::onSourceModelRowsInserted
    );
    QObject::connect(
        &model_, &YADAW::Model::SortFilterProxyListModel::rowsAboutToBeRemoved,
        this, &RegularAudioIOPositionModel::onSourceModelRowsAboutToBeRemoved
    );
    QObject::connect(
        &model_, &YADAW::Model::SortFilterProxyListModel::rowsRemoved,
        this, &RegularAudioIOPositionModel::onSourceModelRowsRemoved
    );
    QObject::connect(
        &model_, &YADAW::Model::SortFilterProxyListModel::dataChanged,
        this, &RegularAudioIOPositionModel::onSourceModelDataChanged
    );
}

RegularAudioIOPositionModel::~RegularAudioIOPositionModel()
{}

int RegularAudioIOPositionModel::itemCount() const
{
    return model_.itemCount();
}

int RegularAudioIOPositionModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant RegularAudioIOPositionModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Position:
            return QVariant::fromValue<QObject*>(
                positions_[row].get()
            );
        case Role::ChannelConfig:
            return model_.data(index, IMixerChannelListModel::Role::ChannelConfig);
        case Role::ChannelCount:
            return model_.data(index, IMixerChannelListModel::Role::ChannelCount);
        }
    }
    return QVariant();
}

int RegularAudioIOPositionModel::findIndexByID(const QString& id) const
{
    auto sourceModel = static_cast<const YADAW::Model::IMixerChannelListModel*>(
        model_.getSourceModel()
    );
    return model_.mapFromSource(sourceModel->getIndexOfId(id));
}

bool RegularAudioIOPositionModel::isComparable(int roleIndex) const
{
    return roleIndex == IAudioIOPositionModel::Role::Position;
}

bool RegularAudioIOPositionModel::isSearchable(int roleIndex) const
{
    return roleIndex == IAudioIOPositionModel::Role::Position;
}

bool RegularAudioIOPositionModel::isFilterable(int roleIndex) const
{
    return roleIndex == IAudioIOPositionModel::Role::ChannelConfig
        || roleIndex == IAudioIOPositionModel::Role::ChannelCount;
}

bool RegularAudioIOPositionModel::isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const
{
    if(roleIndex == IAudioIOPositionModel::Role::Position)
    {
        return positions_[lhs.row()]->getName() < positions_[rhs.row()]->getName();
    }
    return false;
}

bool RegularAudioIOPositionModel::isPassed(const QModelIndex& modelIndex, int role, const QVariant& variant) const
{
    if(isFilterable(role))
    {
        return data(modelIndex, role) == variant;
    }
    return true;
}

bool RegularAudioIOPositionModel::isSearchPassed(int roleIndex, const QModelIndex& modelIndex, const QString& string,
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

const YADAW::Model::SortFilterProxyListModel& RegularAudioIOPositionModel::getModel() const
{
    return model_;
}

void RegularAudioIOPositionModel::onSourceModelRowsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
    beginInsertRows(QModelIndex(), start, end);
}

void RegularAudioIOPositionModel::onSourceModelRowsInserted(
    const QModelIndex& parent, int start, int end)
{
    std::generate_n(
        std::inserter(positions_, positions_.begin() + start),
        end - start + 1,
        [this, i = start]() mutable
        {
            return std::unique_ptr<YADAW::Entity::RegularAudioIOPosition>(
                new YADAW::Entity::RegularAudioIOPosition(*this, i++)
            );
        }
    );
    FOR_RANGE(i, end + 1, positions_.size())
    {
        positions_[i]->updateIndex(i);
    }
    endInsertRows();
}

void RegularAudioIOPositionModel::onSourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void RegularAudioIOPositionModel::onSourceModelRowsRemoved(const QModelIndex& parent, int first, int last)
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

void RegularAudioIOPositionModel::onSourceModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
    const QList<int>& roles)
{
    QList<int> destRoles;
    if(roles.contains(YADAW::Model::IMixerChannelListModel::Role::NameWithIndex))
    {
        FOR_RANGE(i, topLeft.row(), bottomRight.row() + 1)
        {
            positions_[i]->completeNameChanged();
        }
    }
    else if(roles.contains(YADAW::Model::IMixerChannelListModel::Role::Name))
    {
        FOR_RANGE(i, topLeft.row(), bottomRight.row() + 1)
        {
            positions_[i]->nameChanged();
        }
    }
}
}
