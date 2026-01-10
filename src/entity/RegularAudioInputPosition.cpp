#include "RegularAudioInputPosition.hpp"

#include "model/MixerChannelListModel.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Entity
{
RegularAudioInputPosition::RegularAudioInputPosition(
    YADAW::Model::MixerChannelListModel& model,
    std::uint32_t index,
    QObject* parent):
    IAudioIOPosition(parent),
    model_(&model),
    index_(index)
{
    YADAW::Util::setCppOwnership(*this);
    QObject::connect(
        this, &RegularAudioInputPosition::nameChanged,
        this, &RegularAudioInputPosition::completeNameChanged
    );
}

RegularAudioInputPosition::~RegularAudioInputPosition()
{}

IAudioIOPosition::Type RegularAudioInputPosition::getType() const
{
    if(model_->mixer().channelInfoAt(
            YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList, index_
        )->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Audio)
    {
        return IAudioIOPosition::Type::AudioChannelInput;
    }
    else
    {
        return IAudioIOPosition::Type::BusAndFXChannelInput;
    }
}

QString RegularAudioInputPosition::getName() const
{
    return model_->data(
        model_->index(index_),
        YADAW::Model::IMixerChannelListModel::Role::Name
    ).value<QString>();
}

QString RegularAudioInputPosition::getCompleteName() const
{
    return model_->data(
        model_->index(index_),
        YADAW::Model::IMixerChannelListModel::Role::NameWithIndex
    ).value<QString>();
}

const YADAW::Model::MixerChannelListModel& RegularAudioInputPosition::getModel() const
{
    return *model_;
}

std::uint32_t RegularAudioInputPosition::getIndex() const
{
    return index_;
}

void RegularAudioInputPosition::updateIndex(std::uint32_t index)
{
    index_ = index;
}

RegularAudioInputPosition::operator Audio::Mixer::Mixer::Position() const
{
    auto& mixer = model_->mixer();
    return YADAW::Audio::Mixer::Mixer::Position {
        mixer.channelInfoAt(
                YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList, index_
            )->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Audio?
            YADAW::Audio::Mixer::Mixer::Position::Type::AudioChannelInput:
            YADAW::Audio::Mixer::Mixer::Position::Type::FXAndGroupChannelInput,
        *mixer.idAt(YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList, index_)
    };
}
}
