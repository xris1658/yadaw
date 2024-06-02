#include "HardwareAudioIOPositionModel.hpp"

namespace YADAW::Model
{
HardwareAudioIOPositionModel::HardwareAudioIOPositionModel(
    YADAW::Model::MixerChannelListModel& audioHardwareIOModel,
    QObject* parent):
    IAudioIOPositionModel(parent),
    audioHardwareIOModel_(&audioHardwareIOModel)
{
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
        case Role::ID:
            return audioHardwareIOModel_->data(
                audioHardwareIOModel_->index(row),
                MixerChannelListModel::Role::Id
            );
        case Role::Name:
            return audioHardwareIOModel_->data(
                audioHardwareIOModel_->index(row),
                MixerChannelListModel::Role::Name
            );
        case Role::Type:
            return QVariant::fromValue<int>(
                audioHardwareIOModel_->type() == YADAW::Model::MixerChannelListModel::AudioHardwareInput?
                    PositionType::HardwareAudioInput:
                    PositionType::HardwareAudioOutput
            );
        case Role::ChannelConfig:
            return audioHardwareIOModel_->data(
                audioHardwareIOModel_->index(row),
                MixerChannelListModel::Role::ChannelType
            );
        }
    }
    return QVariant();
}

int HardwareAudioIOPositionModel::findIndexByID(const QString& id) const
{
    return audioHardwareIOModel_->getIndexOfId(id);
}

void HardwareAudioIOPositionModel::onSourceModelRowsAboutToBeInserted(
    const QModelIndex& parent, int start, int end)
{
    beginInsertRows(QModelIndex(), start, end);
}

void HardwareAudioIOPositionModel::onSourceModelRowsInserted(
    const QModelIndex& parent, int first, int last)
{
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
            destRoles.append(IAudioIOPositionModel::Role::Name);
        }
        else if(role == YADAW::Model::MixerChannelListModel::Role::Id)
        {
            destRoles.append(IAudioIOPositionModel::Role::ID);
        }
    }
    dataChanged(
        index(topLeft.row()), index(bottomRight.row()), destRoles
    );
}
}
