#include "RegularAudioIOPositionModel.hpp"

#include "model/IMixerChannelListModel.hpp"

namespace YADAW::Model
{
RegularAudioIOPositionModel::RegularAudioIOPositionModel(
    YADAW::Model::IMixerChannelListModel& model,
    YADAW::Model::IMixerChannelListModel::ChannelTypes type, QObject* parent):
    IAudioIOPositionModel(parent),
    model_(model)
{
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
        case Role::ID:
            return model_.data(index, IMixerChannelListModel::Role::Id);
        case Role::Name:
            return model_.data(index, IMixerChannelListModel::Role::Name);
        case Role::Type:
        {
            auto typeAsInt = model_.valueOfFilter(IMixerChannelListModel::Role::ChannelType).value<int>();
            if(typeAsInt == IMixerChannelListModel::ChannelTypes::ChannelTypeAudioFX)
            {
                return QVariant::fromValue<int>(IAudioIOPositionModel::PositionType::AudioFXChannel);
            }
            if(typeAsInt == IMixerChannelListModel::ChannelTypes::ChannelTypeBus)
            {
                return QVariant::fromValue<int>(IAudioIOPositionModel::PositionType::AudioGroupChannel);
            }
            return QVariant();
        }
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
    return roleIndex == IAudioIOPositionModel::Role::Name;
}

bool RegularAudioIOPositionModel::isSearchable(int roleIndex) const
{
    return roleIndex == IAudioIOPositionModel::Role::Name;
}

bool RegularAudioIOPositionModel::isFilterable(int roleIndex) const
{
    return roleIndex == IAudioIOPositionModel::Role::ID
        || roleIndex == IAudioIOPositionModel::Role::Type
        || roleIndex == IAudioIOPositionModel::Role::ChannelConfig
        || roleIndex == IAudioIOPositionModel::Role::ChannelCount;
}

bool RegularAudioIOPositionModel::isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const
{
    if(roleIndex == IAudioIOPositionModel::Role::Name)
    {
        return data(lhs, roleIndex).value<QString>()
            < data(rhs, roleIndex).value<QString>();
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
    if(roleIndex == IAudioIOPositionModel::Role::Name)
    {
        return data(modelIndex, roleIndex).value<QString>().contains(
            string, caseSensitivity
        );
    }
    return true;
}

void RegularAudioIOPositionModel::onSourceModelRowsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
    beginInsertRows(QModelIndex(), start, end);
}

void RegularAudioIOPositionModel::onSourceModelRowsInserted(const QModelIndex& parent, int start, int end)
{
    endInsertRows();
}

void RegularAudioIOPositionModel::onSourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void RegularAudioIOPositionModel::onSourceModelRowsRemoved(const QModelIndex& parent, int first, int last)
{
    endRemoveRows();
}

void RegularAudioIOPositionModel::onSourceModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
    const QList<int>& roles)
{
    QList<int> destRoles;
    for(auto role: roles)
    {
        if(role == YADAW::Model::IMixerChannelListModel::Role::Name)
        {
            destRoles.append(IAudioIOPositionModel::Role::Name);
        }
        else if(role == YADAW::Model::IMixerChannelListModel::Role::Id)
        {
            destRoles.append(IAudioIOPositionModel::Role::ID);
        }
    }
    dataChanged(
        index(topLeft.row()), index(bottomRight.row()), destRoles
    );
}
}
