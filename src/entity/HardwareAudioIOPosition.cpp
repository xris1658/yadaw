#include "HardwareAudioIOPosition.hpp"

#include "model/HardwareAudioIOPositionModel.hpp"
#include "model/MixerChannelListModel.hpp"

#include <QCoreApplication>

namespace YADAW::Entity
{
HardwareAudioIOPosition::HardwareAudioIOPosition(
    YADAW::Model::HardwareAudioIOPositionModel& model,
    std::uint32_t index,
    QObject* parent):
    IAudioIOPosition(parent),
    model_(&model),
    index_(index)
{
    QObject::connect(
        this, &HardwareAudioIOPosition::nameChanged,
        this, &HardwareAudioIOPosition::completeNameChanged
    );
}

HardwareAudioIOPosition::~HardwareAudioIOPosition()
{}

IAudioIOPosition::Type HardwareAudioIOPosition::getType() const
{
    return IAudioIOPosition::Type::AudioHardwareIOChannel;
}

QString HardwareAudioIOPosition::getName() const
{
    return model_->getModel().data(
        model_->index(index_),
        YADAW::Model::IMixerChannelListModel::Role::Name
    ).value<QString>();
}

QString HardwareAudioIOPosition::getCompleteName() const
{
    return QString("%1: %2").arg(QString::number(index_ + 1), getName());
}

const YADAW::Model::HardwareAudioIOPositionModel& HardwareAudioIOPosition::getModel() const
{
    return *model_;
}

std::uint32_t HardwareAudioIOPosition::getIndex() const
{
    return index_;
}

void HardwareAudioIOPosition::updateIndex(std::uint32_t index)
{
    index_ = index;
}

HardwareAudioIOPosition::operator Audio::Mixer::Mixer::Position() const
{
    auto& mixerChannelListModel = model_->getModel();
    auto idAsString = mixerChannelListModel.data(
        mixerChannelListModel.index(getIndex()),
        YADAW::Model::MixerChannelListModel::Role::Id
    ).value<QString>();
    return YADAW::Audio::Mixer::Mixer::Position {
        .type = YADAW::Audio::Mixer::Mixer::Position::AudioHardwareIOChannel,
        .id = idAsString.toULongLong()
    };
}
}
