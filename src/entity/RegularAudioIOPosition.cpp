#include "RegularAudioIOPosition.hpp"

#include "model/MixerChannelListModel.hpp"
#include "model/RegularAudioIOPositionModel.hpp"
#include "util/QmlUtil.hpp"

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
    YADAW::Util::setCppOwnership(*this);
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
    auto channelType = mixerChannelListModel.valueOfFilter(
        YADAW::Model::MixerChannelListModel::Role::ChannelType
    ).value<int>();
    auto idAsString = mixerChannelListModel.data(
        mixerChannelListModel.index(getIndex()),
        YADAW::Model::MixerChannelListModel::Role::Id
    );
    return YADAW::Audio::Mixer::Mixer::Position {
        channelType == YADAW::Model::MixerChannelListModel::ChannelTypes::ChannelTypeAudio?
            YADAW::Audio::Mixer::Mixer::Position::AudioChannelInput:
            YADAW::Audio::Mixer::Mixer::Position::FXAndGroupChannelInput,
        idAsString.toULongLong()
    };
}
}
