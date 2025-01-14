#include "MixerChannelSendListModel.hpp"

#include "entity/RegularAudioIOPosition.hpp"
#include "util/Base.hpp"

namespace YADAW::Model
{
using GetSendCount = decltype(&YADAW::Audio::Mixer::Mixer::channelSendCount);

GetSendCount getSendCount[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputChannelSendCount,
    &YADAW::Audio::Mixer::Mixer::channelSendCount,
    &YADAW::Audio::Mixer::Mixer::audioOutputChannelSendCount,
};

using GetSendDestination = decltype(&YADAW::Audio::Mixer::Mixer::channelSendDestination);

GetSendDestination getSendDestination[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputChannelSendDestination,
    &YADAW::Audio::Mixer::Mixer::channelSendDestination,
    &YADAW::Audio::Mixer::Mixer::audioOutputChannelSendDestination,
};

using SetSendDestination = decltype(&YADAW::Audio::Mixer::Mixer::setChannelSendDestination);

SetSendDestination setSendDestination[3] = {
    &Audio::Mixer::Mixer::setAudioInputChannelSendDestination,
    &Audio::Mixer::Mixer::setChannelSendDestination,
    &Audio::Mixer::Mixer::setAudioOutputChannelSendDestination,
};

using GetSendFader = decltype(&YADAW::Audio::Mixer::Mixer::channelSendFaderAt);

GetSendFader getSendFader[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputChannelSendFaderAt,
    &YADAW::Audio::Mixer::Mixer::channelSendFaderAt,
    &YADAW::Audio::Mixer::Mixer::audioOutputChannelSendFaderAt,
};

using GetConstSendMute = OptionalRef<const YADAW::Audio::Util::Mute> (YADAW::Audio::Mixer::Mixer::*)(
    uint32_t channelIndex,
    uint32_t sendIndex) const;

GetConstSendMute getConstSendMute[3] = {
    static_cast<GetConstSendMute>(&YADAW::Audio::Mixer::Mixer::audioInputChannelSendMuteAt),
    static_cast<GetConstSendMute>(&YADAW::Audio::Mixer::Mixer::channelSendMuteAt),
    static_cast<GetConstSendMute>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelSendMuteAt),
};

using GetSendMute = OptionalRef<YADAW::Audio::Util::Mute> (YADAW::Audio::Mixer::Mixer::*)(
    uint32_t channelIndex,
    uint32_t sendIndex);

GetSendMute getSendMute[3] = {
    static_cast<GetSendMute>(&YADAW::Audio::Mixer::Mixer::audioInputChannelSendMuteAt),
    static_cast<GetSendMute>(&YADAW::Audio::Mixer::Mixer::channelSendMuteAt),
    static_cast<GetSendMute>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelSendMuteAt),
};

using GetConstSendPolarityInverter = OptionalRef<const YADAW::Audio::Mixer::PolarityInverter>(YADAW::Audio::Mixer::Mixer::*)(
    std::uint32_t channelIndex,
    std::uint32_t sendIndex) const;

GetConstSendPolarityInverter getConstSendPolarityInverter[3] = {
    static_cast<GetConstSendPolarityInverter>(&YADAW::Audio::Mixer::Mixer::audioInputChannelSendPolarityInverterAt),
    static_cast<GetConstSendPolarityInverter>(&YADAW::Audio::Mixer::Mixer::channelSendPolarityInverterAt),
    static_cast<GetConstSendPolarityInverter>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelSendPolarityInverterAt),
};

using GetSendPolarityInverter = OptionalRef<YADAW::Audio::Mixer::PolarityInverter>(YADAW::Audio::Mixer::Mixer::*)(
    std::uint32_t channelIndex,
    std::uint32_t sendIndex);

GetSendPolarityInverter getSendPolarityInverter[3] = {
    static_cast<GetSendPolarityInverter>(&YADAW::Audio::Mixer::Mixer::audioInputChannelSendPolarityInverterAt),
    static_cast<GetSendPolarityInverter>(&YADAW::Audio::Mixer::Mixer::channelSendPolarityInverterAt),
    static_cast<GetSendPolarityInverter>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelSendPolarityInverterAt),
};

using IsSendPreFader = decltype(&YADAW::Audio::Mixer::Mixer::channelSendIsPreFader);

IsSendPreFader isSendPreFader[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputChannelSendIsPreFader,
    &YADAW::Audio::Mixer::Mixer::channelSendIsPreFader,
    &YADAW::Audio::Mixer::Mixer::audioOutputChannelSendIsPreFader,
};

using SetSendPreFader = decltype(&YADAW::Audio::Mixer::Mixer::setChannelSendPreFader);

SetSendPreFader setSendPreFader[3] = {
    &YADAW::Audio::Mixer::Mixer::setAudioInputChannelSendPreFader,
    &YADAW::Audio::Mixer::Mixer::setChannelSendPreFader,
    &YADAW::Audio::Mixer::Mixer::setAudioOutputChannelSendPreFader,
};

MixerChannelSendListModel::MixerChannelSendListModel(
    YADAW::Audio::Mixer::Mixer& mixer,
    YADAW::Model::MixerChannelListModel::ListType type,
    std::uint32_t channelIndex):
    mixer_(&mixer),
    listType_(type),
    channelIndex_(channelIndex)
{}

MixerChannelSendListModel::~MixerChannelSendListModel()
{}

int MixerChannelSendListModel::itemCount() const
{
    return *(mixer_->*getSendCount[YADAW::Util::underlyingValue(listType_)])(channelIndex_);
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
                (mixer_->*getConstSendMute[YADAW::Util::underlyingValue(listType_)])(channelIndex_, row)->get().getMute()
            );
        case Role::PolarityInverter:
            return QVariant::fromValue<QObject*>(
                polarityInverterModels_[row].get()
            );
        case Role::IsPreFader:
            return QVariant::fromValue<bool>(
                *(mixer_->*isSendPreFader[YADAW::Util::underlyingValue(listType_)])(channelIndex_, row)
            );
        case Role::Volume:
            return QVariant::fromValue<double>(
                (mixer_->*getSendFader[YADAW::Util::underlyingValue(listType_)])(
                    channelIndex_, row
                )->get().parameter(0)->value()
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
                    return *(mixer_->*setSendDestination[YADAW::Util::underlyingValue(listType_)])(
                        channelIndex_, row,
                        static_cast<YADAW::Audio::Mixer::Mixer::Position>(
                            regularAudioIOPosition
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
            (mixer_->*getSendMute[YADAW::Util::underlyingValue(listType_)])(channelIndex_, row)->get().setMute(value.value<bool>());
            return true;
        }
        case Role::IsPreFader:
        {
            return *(mixer_->*setSendPreFader[YADAW::Util::underlyingValue(listType_)])(
                channelIndex_, row, value.value<bool>()
            );
        }
        }
    }
    return false;
}

bool MixerChannelSendListModel::append(bool isPreFader, YADAW::Entity::IAudioIOPosition* position)
{
    return false;
}

bool MixerChannelSendListModel::remove(int position, int removeCount)
{
    return false;
}

void MixerChannelSendListModel::setChannelIndex(std::uint32_t channelIndex)
{
    channelIndex_ = channelIndex;
}
}
