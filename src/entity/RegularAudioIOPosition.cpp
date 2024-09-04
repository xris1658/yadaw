#include "RegularAudioIOPosition.hpp"

#include "model/MixerChannelListModel.hpp"
#include "model/RegularAudioIOPositionModel.hpp"

namespace YADAW::Entity
{
RegularAudioIOPosition::RegularAudioIOPosition(
    YADAW::Model::RegularAudioIOPositionModel& model,
    std::uint32_t index,
    QObject* parent):
    IAudioIOPosition(parent),
    model_(&model),
    index_(index)
{
    QObject::connect(
        this, &RegularAudioIOPosition::nameChanged,
        this, &RegularAudioIOPosition::completeNameChanged
    );
}

RegularAudioIOPosition::~RegularAudioIOPosition()
{}

IAudioIOPosition::Type RegularAudioIOPosition::getType() const
{
    return IAudioIOPosition::Type::BusAndFXChannel;
}

QString RegularAudioIOPosition::getName() const
{
    return model_->getModel().data(
        model_->index(index_),
        YADAW::Model::IMixerChannelListModel::Role::Name
    ).value<QString>();
}

QString RegularAudioIOPosition::getCompleteName() const
{
    return model_->getModel().data(
        model_->index(index_),
        YADAW::Model::IMixerChannelListModel::Role::NameWithIndex
    ).value<QString>();
}

const YADAW::Model::RegularAudioIOPositionModel& RegularAudioIOPosition::getModel() const
{
    return *model_;
}

std::uint32_t RegularAudioIOPosition::getIndex() const
{
    return index_;
}

void RegularAudioIOPosition::updateIndex(std::uint32_t index)
{
    index_ = index;
}

RegularAudioIOPosition::operator Audio::Mixer::Mixer::Position() const
{
    auto& mixerChannelListModel = model_->getModel();
    auto idAsString = mixerChannelListModel.data(
        mixerChannelListModel.index(getIndex()),
        YADAW::Model::MixerChannelListModel::Role::Id
    );
    return YADAW::Audio::Mixer::Mixer::Position {
        YADAW::Audio::Mixer::Mixer::Position::BusAndFXChannel, 0,
        idAsString.toULongLong()
    };
}
}
