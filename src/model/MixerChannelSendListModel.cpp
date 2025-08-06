#include "MixerChannelSendListModel.hpp"

#include "entity/HardwareAudioIOPosition.hpp"
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
    listType_(
        type == YADAW::Audio::Mixer::Mixer::ChannelListType::AudioHardwareInputList?
            YADAW::Model::MixerChannelListModel::ListType::AudioHardwareInput:
        type == YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList?
            YADAW::Model::MixerChannelListModel::ListType::Regular:
        YADAW::Model::MixerChannelListModel::ListType::AudioHardwareOutput
    ),
    channelIndex_(channelIndex)
{}

MixerChannelSendListModel::~MixerChannelSendListModel()
{}

int MixerChannelSendListModel::itemCount() const
{
    return *(mixer_->sendCount(channelListType_, channelIndex_));
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
                if(auto type = pPosition->getType(); type == YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel)
                {
                    const auto& regularAudioIOPosition = static_cast<const YADAW::Entity::RegularAudioIOPosition&>(*pPosition);
                    return *(
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
                    // not implemented
                    return false;
                }
            }
        }
        case Role::Mute:
        {
            mixer_->sendMuteAt(channelListType_, channelIndex_, row)->get().setMute(value.value<bool>());
            return true;
        }
        case Role::IsPreFader:
        {
            return *(
                mixer_->setSendPreFader(
                    channelListType_, channelIndex_, row, value.value<bool>()
                )
            );
        }
        }
    }
    return false;
}

bool MixerChannelSendListModel::append(bool isPreFader, YADAW::Entity::IAudioIOPosition* position)
{
    auto ret = false;
    if(listType_ == MixerChannelListModel::ListType::Regular)
    {
        if(position)
        {
            switch(position->getType())
            {
            case YADAW::Entity::IAudioIOPosition::Type::AudioHardwareIOChannel:
            {
                const auto& hardwareAudioIOPosition = static_cast<const YADAW::Entity::HardwareAudioIOPosition&>(*position);
                ret = *mixer_->appendChannelSend(
                    channelIndex_, isPreFader, hardwareAudioIOPosition
                );
                break;
            }
            case YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel:
            {
                const auto& regularAudioIOPosition = static_cast<const YADAW::Entity::RegularAudioIOPosition&>(*position);
                ret = *mixer_->appendChannelSend(
                    channelIndex_, isPreFader, regularAudioIOPosition
                );
                break;
            }
            case YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO:
            {
                // not implemented
                break;
            }
        }
        }
    }
    if(ret)
    {
        auto oldItemCount = itemCount() - 1;
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
    if(listType_ == MixerChannelListModel::ListType::Regular
        && position >= 0 && removeCount > 0 && position + removeCount <= itemCount())
    {
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        mixer_->removeChannelSend(channelIndex_, position, removeCount);
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
        endRemoveRows();
        return true;
    }
    return false;
}

void MixerChannelSendListModel::setChannelIndex(std::uint32_t channelIndex)
{
    channelIndex_ = channelIndex;
}
}
