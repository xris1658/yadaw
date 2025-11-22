#include "MixerChannelSendListModel.hpp"

#include "entity/HardwareAudioIOPosition.hpp"
#include "entity/PluginAuxAudioIOPosition.hpp"
#include "entity/RegularAudioIOPosition.hpp"
#include "util/Base.hpp"

namespace YADAW::Model
{
MixerChannelSendListModel::MixerChannelSendListModel(
    YADAW::Audio::Mixer::Mixer& mixer,
    YADAW::Audio::Mixer::Mixer::ChannelListType type,
    std::uint32_t channelIndex):
    mixer_(&mixer),
    channelListType_(type),
    channelIndex_(channelIndex)
{
    QObject::connect(
        this, &MixerChannelSendListModel::appended,
        this, &MixerChannelSendListModel::onAppended
    );
    QObject::connect(
        this, &MixerChannelSendListModel::removed,
        this, &MixerChannelSendListModel::onRemoved
    );
}

MixerChannelSendListModel::~MixerChannelSendListModel()
{}

int MixerChannelSendListModel::itemCount() const
{
    return destinations_.size();
}

OptionalRef<const YADAW::Entity::SendPosition>
MixerChannelSendListModel::positionAt(std::uint32_t index) const
{
    if(index < itemCount())
    {
        return *sendPositions_[index];
    }
    return std::nullopt;
}

OptionalRef<YADAW::Entity::SendPosition> MixerChannelSendListModel::positionAt(std::uint32_t index)
{
    if(index < itemCount())
    {
        return *sendPositions_[index];
    }
    return std::nullopt;
}

int MixerChannelSendListModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant MixerChannelSendListModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Destination:
            return QVariant::fromValue<QObject*>(destinations_[row]);
        case Role::Mute:
            return QVariant::fromValue<bool>(
                mixer_->sendMuteAt(channelListType_, channelIndex_, row)->get().getMute()
            );
        case Role::PolarityInverter:
            return QVariant::fromValue<QObject*>(
                polarityInverterModels_[row].get()
            );
        case Role::IsPreFader:
            return QVariant::fromValue<bool>(
                *(mixer_->sendIsPreFader(channelListType_, channelIndex_, row))
            );
        case Role::Volume:
            return QVariant::fromValue<double>(
                mixer_->sendFaderAt(channelListType_, channelIndex_, row)->get().parameter(0)->value()
            );
        case Role::EditingVolume:
            return QVariant::fromValue<bool>(editingVolume_[row]);
        }
    }
    return {};
}

bool MixerChannelSendListModel::setData(
    const QModelIndex& index, const QVariant& value, int role)
{
    if(auto row = index.row(); row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Destination:
        {
            if(auto pPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(value.value<QObject*>()))
            {
                position_ = pPosition;
                if(auto type = pPosition->getType(); type == YADAW::Entity::IAudioIOPosition::Type::AudioHardwareIOChannel)
                {
                    const auto& hardwareAudioIOPosition = static_cast<const YADAW::Entity::HardwareAudioIOPosition&>(*pPosition);
                    return mixer_->setSendDestination(
                        channelListType_, channelIndex_, row,
                        static_cast<YADAW::Audio::Mixer::Mixer::Position>(
                            hardwareAudioIOPosition
                        )
                    ).value_or(false);
                }
                else if(type == YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel)
                {
                    const auto& regularAudioIOPosition = static_cast<const YADAW::Entity::RegularAudioIOPosition&>(*pPosition);
                    return mixer_->setSendDestination(
                        channelListType_, channelIndex_, row,
                        static_cast<YADAW::Audio::Mixer::Mixer::Position>(
                            regularAudioIOPosition
                        )
                    ).value_or(false);
                }
                else if(type == YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO)
                {
                    const auto& pluginAuxAudioIOPosition = static_cast<const YADAW::Entity::PluginAuxAudioIOPosition&>(*pPosition);
                    return mixer_->setSendDestination(
                        channelListType_, channelIndex_, row,
                        static_cast<YADAW::Audio::Mixer::Mixer::Position>(
                        pluginAuxAudioIOPosition
                        )
                    ).value_or(false);
                }
            }
            return false;
        }
        case Role::Mute:
        {
            mixer_->sendMuteAt(channelListType_, channelIndex_, row)->get().setMute(value.value<bool>());
            dataChanged(index, index, {Role::Mute});
            return true;
        }
        case Role::IsPreFader:
        {
            auto ret = *(
                mixer_->setSendPreFader(
                    channelListType_, channelIndex_, row, value.value<bool>()
                )
            );
            if(ret)
            {
                dataChanged(index, index, {Role::IsPreFader});
            }
            return ret;
        }
        }
    }
    return false;
}

bool MixerChannelSendListModel::append(bool isPreFader, YADAW::Entity::IAudioIOPosition* position)
{
    if(position)
    {
        position_ = position;
        switch(position->getType())
        {
        case YADAW::Entity::IAudioIOPosition::Type::AudioHardwareIOChannel:
        {
            const auto& hardwareAudioIOPosition = static_cast<const YADAW::Entity::HardwareAudioIOPosition&>(*position);
            return *mixer_->appendSend(
                channelListType_, channelIndex_, isPreFader, hardwareAudioIOPosition
            );
            break;
        }
        case YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel:
        {
            const auto& regularAudioIOPosition = static_cast<const YADAW::Entity::RegularAudioIOPosition&>(*position);
            return *mixer_->appendSend(
                channelListType_, channelIndex_, isPreFader, regularAudioIOPosition
            );
        }
        case YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO:
        {
            const auto& pluginAuxAudioIOPosition = static_cast<const YADAW::Entity::PluginAuxAudioIOPosition&>(*position);
            return *mixer_->appendSend(
                channelListType_, channelIndex_, isPreFader, pluginAuxAudioIOPosition
            );
        }
        }
    }
    return false;
}

bool MixerChannelSendListModel::remove(int position, int removeCount)
{
    if(position >= 0 && removeCount > 0 && position + removeCount <= itemCount())
    {
        return mixer_->removeSend(channelListType_, channelIndex_, position, removeCount).value_or(false);
    }
    return false;
}

const YADAW::Audio::Mixer::Mixer& MixerChannelSendListModel::mixer() const
{
    return *mixer_;
}

YADAW::Audio::Mixer::Mixer& MixerChannelSendListModel::mixer()
{
    return *mixer_;
}

YADAW::Audio::Mixer::Mixer::ChannelListType MixerChannelSendListModel::channelListType() const
{
    return channelListType_;
}

std::uint32_t MixerChannelSendListModel::channelIndex() const
{
    return channelIndex_;
}

void MixerChannelSendListModel::setChannelIndex(std::uint32_t channelIndex)
{
    channelIndex_ = channelIndex;
    for(auto& sendPosition: sendPositions_)
    {
        sendPosition->nameChanged();
    }
}

void MixerChannelSendListModel::onAppended()
{
    auto oldItemCount = itemCount();
    sendPositions_.emplace_back(
        std::make_unique<YADAW::Entity::SendPosition>(*this, oldItemCount)
    );
    beginInsertRows(QModelIndex(), oldItemCount, oldItemCount);
    editingVolume_.emplace_back(false);
    destinations_.emplace_back(position_);
    polarityInverterModels_.emplace_back(
        std::make_unique<YADAW::Model::PolarityInverterModel>(
            mixer_->sendPolarityInverterAt(channelListType_, channelIndex_, oldItemCount)->get()
        )
    );
    endInsertRows();
}

void MixerChannelSendListModel::onDestinationChanged(int index)
{
    if(index >= 0 && index < destinations_.size())
    {
        destinations_[index] = position_;
        dataChanged(this->index(index), this->index(index), {Role::Destination});
    }
}

void MixerChannelSendListModel::onRemoved(int first, int last)
{
    auto position = first;
    auto removeCount = last - first + 1;
    beginRemoveRows(QModelIndex(), first, last);
    editingVolume_.erase(
        editingVolume_.begin() + position,
        editingVolume_.begin() + last + 1
    );
    destinations_.erase(
        destinations_.begin() + position,
        destinations_.begin() + last
    );
    polarityInverterModels_.erase(
        polarityInverterModels_.begin() + position,
        polarityInverterModels_.begin() + last
    );
    sendPositions_.erase(
        sendPositions_.begin() + position,
        sendPositions_.begin() + last
    );
    FOR_RANGE(i, position, sendPositions_.size())
    {
        sendPositions_[i]->updateSendIndex(i);
    }
    endRemoveRows();
}
}
