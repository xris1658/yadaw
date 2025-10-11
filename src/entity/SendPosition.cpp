#include "SendPosition.hpp"

#include "model/MixerChannelSendListModel.hpp"

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
    return getCompleteName(); // TODO?
}

constexpr char listTypeIndicator[3][2] = {"I", "R", "O"};

QString SendPosition::getCompleteName() const
{
    auto type = model_->channelListType();
    auto channelIndex = model_->channelIndex();
    auto sendIndex = index_;
    QString ret;
    ret.append(listTypeIndicator[type]);
    ret.append(QString::number(channelIndex + 1));
    ret.append("Send");
    ret.append(QString::number(sendIndex + 1));
    return ret;
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
        .type = YADAW::Audio::Mixer::Mixer::Position::Type::Send,
        .id = model_->mixer().sendID(model_->channelListType(), model_->channelIndex(), index_).value()
    };
}
}
