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
{}

MixerChannelSendListModel::~MixerChannelSendListModel()
{}

int MixerChannelSendListModel::itemCount() const
{
    return *(mixer_->sendCount(channelListType_, channelIndex_));
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
            auto ret = false;
            if(auto pPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(value.value<QObject*>()))
            {
                if(auto type = pPosition->getType(); type == YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel)
                {
                    const auto& regularAudioIOPosition = static_cast<const YADAW::Entity::RegularAudioIOPosition&>(*pPosition);
                    ret = *(
                            mixer_->setSendDestination(
                            channelListType_, channelIndex_, row,
                            static_cast<YADAW::Audio::Mixer::Mixer::Position>(
                                regularAudioIOPosition
                            )
                        )
                    );
                }
                else if(type == YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO)
                {
                    const auto& pluginAuxAudioIOPosition = static_cast<const YADAW::Entity::PluginAuxAudioIOPosition&>(*pPosition);
                    ret = *(
                        mixer_->setSendDestination(
                            channelListType_, channelIndex_, row,
                            static_cast<YADAW::Audio::Mixer::Mixer::Position>(
                                pluginAuxAudioIOPosition
                            )
                        )
                    );
                }
            }
            if(ret)
            {
                destinationAboutToBeChanged(row, row);
                dataChanged(index, index, {Role::Destination});
            }
            return ret;
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
    auto ret = false;
    if(position)
    {
        switch(position->getType())
        {
        case YADAW::Entity::IAudioIOPosition::Type::AudioHardwareIOChannel:
        {
            const auto& hardwareAudioIOPosition = static_cast<const YADAW::Entity::HardwareAudioIOPosition&>(*position);
            ret = *mixer_->appendSend(
                channelListType_, channelIndex_, isPreFader, hardwareAudioIOPosition
            );
            break;
        }
        case YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel:
        {
            const auto& regularAudioIOPosition = static_cast<const YADAW::Entity::RegularAudioIOPosition&>(*position);
            ret = *mixer_->appendSend(
                channelListType_, channelIndex_, isPreFader, regularAudioIOPosition
            );
            break;
        }
        case YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO:
        {
            const auto& pluginAuxAudioIOPosition = static_cast<const YADAW::Entity::PluginAuxAudioIOPosition&>(*position);
            ret = *mixer_->appendSend(
                channelListType_, channelIndex_, isPreFader, pluginAuxAudioIOPosition
            );
            break;
        }
        }
    }
    if(ret)
    {
        auto oldItemCount = itemCount() - 1;
        sendPositions_.emplace_back(
            std::make_unique<YADAW::Entity::SendPosition>(*this, oldItemCount)
        );
        beginInsertRows(QModelIndex(), oldItemCount, oldItemCount);
        editingVolume_.emplace_back(false);
        destinations_.emplace_back(position);
        polarityInverterModels_.emplace_back(
            std::make_unique<YADAW::Model::PolarityInverterModel>(
                mixer_->sendPolarityInverterAt(channelListType_, channelIndex_, oldItemCount)->get()
            )
        );
        endInsertRows();
    }
    return ret;
}

bool MixerChannelSendListModel::remove(int position, int removeCount)
{
    if(position >= 0 && removeCount > 0 && position + removeCount <= itemCount())
    {
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        mixer_->removeSend(channelListType_, channelIndex_, position, removeCount);
        editingVolume_.erase(
            editingVolume_.begin() + position,
            editingVolume_.begin() + position + removeCount
        );
        destinations_.erase(
            destinations_.begin() + position,
            destinations_.begin() + position + removeCount
        );
        polarityInverterModels_.erase(
            polarityInverterModels_.begin() + position,
            polarityInverterModels_.begin() + position + removeCount
        );
        sendPositions_.erase(
            sendPositions_.begin() + position,
            sendPositions_.begin() + position + removeCount
        );
        FOR_RANGE(i, position, sendPositions_.size())
        {
            sendPositions_[i]->updateSendIndex(i);
        }
        endRemoveRows();
        return true;
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
}
