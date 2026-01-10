#include "RegularAudioOutputPosition.hpp"

#include "model/MixerChannelListModel.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Entity
{
RegularAudioOutputPosition::RegularAudioOutputPosition(
    YADAW::Model::MixerChannelListModel& model,
    std::uint32_t index,
    QObject* parent):
    IAudioIOPosition(parent),
    model_(&model),
    index_(index)
{
    YADAW::Util::setCppOwnership(*this);
    QObject::connect(
        this, &RegularAudioOutputPosition::nameChanged,
        this, &RegularAudioOutputPosition::completeNameChanged
    );
}

RegularAudioOutputPosition::~RegularAudioOutputPosition()
{}

IAudioIOPosition::Type RegularAudioOutputPosition::getType() const
{
    return IAudioIOPosition::Type::RegularChannelOut;
}

QString RegularAudioOutputPosition::getName() const
{
    return model_->data(
        model_->index(index_),
        YADAW::Model::IMixerChannelListModel::Role::Name
    ).value<QString>();
}

QString RegularAudioOutputPosition::getCompleteName() const
{
    return model_->data(
        model_->index(index_),
        YADAW::Model::IMixerChannelListModel::Role::NameWithIndex
    ).value<QString>();
}

const YADAW::Model::MixerChannelListModel& RegularAudioOutputPosition::getModel() const
{
    return *model_;
}

std::uint32_t RegularAudioOutputPosition::getIndex() const
{
    return index_;
}

void RegularAudioOutputPosition::updateIndex(std::uint32_t index)
{
    index_ = index;
}

RegularAudioOutputPosition::operator Audio::Mixer::Mixer::Position() const
{
    auto& mixer = model_->mixer();
    return YADAW::Audio::Mixer::Mixer::Position {
        YADAW::Audio::Mixer::Mixer::Position::Type::RegularChannelOutput,
        *mixer.idAt(YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList, index_)
    };
}
}
