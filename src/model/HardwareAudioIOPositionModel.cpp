#include "HardwareAudioIOPositionModel.hpp"

namespace YADAW::Model
{
HardwareAudioIOPositionModel::HardwareAudioIOPositionModel(
    YADAW::Model::MixerChannelListModel& audioHardwareIOModel,
    QObject* parent):
    IAudioIOPositionModel(parent),
    audioHardwareIOModel_(&audioHardwareIOModel)
{}

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
}
