#include "SendPosition.hpp"

namespace YADAW::Entity
{
SendPosition::SendPosition(
    YADAW::Model::MixerChannelSendListModel& model, std::uint32_t index):
    model_(&model), index_(index)
{
}

SendPosition::~SendPosition()
{
}

IAudioIOPosition::Type SendPosition::getType() const
{
    return IAudioIOPosition::Type::Send;
}

QString SendPosition::getName() const
{
    return {};
}

QString SendPosition::getCompleteName() const
{
    return {};
}

const YADAW::Model::MixerChannelSendListModel& SendPosition::getModel() const
{
    return *model_;
}

YADAW::Model::MixerChannelSendListModel& SendPosition::getModel()
{
    return *model_;
}

void SendPosition::updateSendIndex(std::uint32_t index)
{
    index_ = index;
    nameChanged();
}

SendPosition::operator YADAW::Audio::Mixer::Mixer::Position() const
{
    return YADAW::Audio::Mixer::Mixer::Position {
        .type = YADAW::Audio::Mixer::Mixer::Position::Type::Send
        // TODO
    };
}
}
