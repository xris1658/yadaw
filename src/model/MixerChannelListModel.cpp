#include "MixerChannelListModel.hpp"

#include "controller/AudioEngineController.hpp"
#include "entity/ChannelConfigHelper.hpp"
#include "model/MixerChannelInsertListModel.hpp"
#include "util/Base.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
YADAW::Audio::Mixer::Mixer::ChannelType channelTypeFromModelTypes(
    IMixerChannelListModel::ChannelTypes type
)
{
    return static_cast<YADAW::Audio::Mixer::Mixer::ChannelType>(type);
}

using GetCount = decltype(&YADAW::Audio::Mixer::Mixer::channelCount);

GetCount getCount[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputChannelCount,
    &YADAW::Audio::Mixer::Mixer::channelCount,
    &YADAW::Audio::Mixer::Mixer::audioOutputChannelCount,
};

using GetInserts =
    OptionalRef<YADAW::Audio::Mixer::Inserts>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t);

GetInserts getPreFaderInserts[3] = {
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPreFaderInsertsAt),
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::channelPreFaderInsertsAt),
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPreFaderInsertsAt)
};

GetInserts getPostFaderInserts[3] = {
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPostFaderInsertsAt),
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::channelPostFaderInsertsAt),
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPostFaderInsertsAt)
};

using GetConstInserts =
    OptionalRef<const YADAW::Audio::Mixer::Inserts>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t) const;

GetConstInserts getConstPreFaderInserts[3] = {
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPreFaderInsertsAt),
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::channelPreFaderInsertsAt),
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPreFaderInsertsAt)
};

GetConstInserts getConstPostFaderInserts[3] = {
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPostFaderInsertsAt),
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::channelPostFaderInsertsAt),
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPostFaderInsertsAt)
};

using GetChannelInfo =
    OptionalRef<YADAW::Audio::Mixer::Mixer::ChannelInfo>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t);

GetChannelInfo getChannelInfo[3] = {
    static_cast<GetChannelInfo>(&YADAW::Audio::Mixer::Mixer::audioInputChannelInfoAt),
    static_cast<GetChannelInfo>(&YADAW::Audio::Mixer::Mixer::channelInfoAt),
    static_cast<GetChannelInfo>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelInfoAt)
};

using GetConstChannelInfo =
    OptionalRef<const YADAW::Audio::Mixer::Mixer::ChannelInfo>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t) const;

GetConstChannelInfo getConstChannelInfo[3] = {
    static_cast<GetConstChannelInfo>(&YADAW::Audio::Mixer::Mixer::audioInputChannelInfoAt),
    static_cast<GetConstChannelInfo>(&YADAW::Audio::Mixer::Mixer::channelInfoAt),
    static_cast<GetConstChannelInfo>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelInfoAt)
};

using GetMute =
    std::optional<bool>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t) const;

GetMute getMute[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputMuted,
    &YADAW::Audio::Mixer::Mixer::muted,
    &YADAW::Audio::Mixer::Mixer::audioOutputMuted
};

using SetMute =
    void(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t, bool);

SetMute setMute[3] = {
    &YADAW::Audio::Mixer::Mixer::setAudioInputMuted,
    &YADAW::Audio::Mixer::Mixer::setMuted,
    &YADAW::Audio::Mixer::Mixer::setAudioOutputMuted
};

using RemoveChannels =
    decltype(&YADAW::Audio::Mixer::Mixer::removeChannel);

RemoveChannels removeChannels[3] = {
    &YADAW::Audio::Mixer::Mixer::removeAudioInputChannel,
    &YADAW::Audio::Mixer::Mixer::removeChannel,
    &YADAW::Audio::Mixer::Mixer::removeAudioOutputChannel
};

using ClearChannels =
    decltype(&YADAW::Audio::Mixer::Mixer::clearChannels);

ClearChannels clearChannels[3] = {
    &YADAW::Audio::Mixer::Mixer::clearAudioInputChannels,
    &YADAW::Audio::Mixer::Mixer::clearChannels,
    &YADAW::Audio::Mixer::Mixer::clearAudioOutputChannels,
};

IMixerChannelListModel::ChannelTypes getChannelType(YADAW::Audio::Mixer::Mixer::ChannelType type)
{
    return static_cast<IMixerChannelListModel::ChannelTypes>(YADAW::Util::underlyingValue(type));
}

MixerChannelListModel::MixerChannelListModel(
    YADAW::Audio::Mixer::Mixer& mixer, ListType listType, QObject* parent):
    IMixerChannelListModel(parent),
    mixer_(mixer),
    listType_(listType)
{
    auto count = itemCount();
    insertModels_.reserve(count);
    FOR_RANGE0(i, count)
    {
        insertModels_.emplace_back(
            std::make_unique<YADAW::Model::MixerChannelInsertListModel>(
                (mixer_.*getPreFaderInserts[YADAW::Util::underlyingValue(listType_)])(i)->get(),
                listType_,
                i,
                true,
                0
            )
        );
    }
}

MixerChannelListModel::~MixerChannelListModel()
{}

int MixerChannelListModel::itemCount() const
{
    return (mixer_.*getCount[YADAW::Util::underlyingValue(listType_)])();
}

int MixerChannelListModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant MixerChannelListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
    switch(role)
    {
    case Role::Name:
    {
        const auto& optionalInfo = (mixer_.*getConstChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
        if(optionalInfo.has_value())
        {
            return QVariant::fromValue(optionalInfo->get().name);
        }
        return QVariant();
    }
    case Role::Color:
    {
        const auto& optionalInfo = (mixer_.*getConstChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
        if(optionalInfo.has_value())
        {
            return QVariant::fromValue(optionalInfo->get().color);
        }
        return QVariant();
    }
    case Role::ChannelType:
    {
        auto optionalInfo = (mixer_.*getConstChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
        if(optionalInfo.has_value())
        {
            return QVariant::fromValue(getChannelType(optionalInfo->get().channelType));
        }
        return QVariant();
    }
    case Role::Inserts:
    {
        return QVariant::fromValue<QObject*>(insertModels_[row].get());
    }
    case Role::Mute:
    {
        return QVariant::fromValue<bool>(
            *(mixer_.*getMute[YADAW::Util::underlyingValue(listType_)])(row)
        );
    }
    }
    }
    return QVariant();
}

bool MixerChannelListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
        {
            auto optionalInfo = (mixer_.*getChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                optionalInfo->get().name = value.value<QString>();
                dataChanged(index, index, {Role::Name});
                return true;
            }
            return false;
        }
        case Role::Color:
        {
            auto optionalInfo = (mixer_.*getChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                optionalInfo->get().color = value.value<QColor>();
                dataChanged(index, index, {Role::Color});
                return true;
            }
            return false;
        }
        case Role::Mute:
        {
            (mixer_.*setMute[YADAW::Util::underlyingValue(listType_)])(row, value.value<bool>());
            dataChanged(index, index, {Role::Mute});
            return true;
        }
        }
    }
    return false;
}

bool MixerChannelListModel::insert(int position, IMixerChannelListModel::ChannelTypes type,
    YADAW::Entity::ChannelConfig::Config channelConfig,
    int channelCount)
{
    if(listType_ == ListType::Regular)
    {
        auto ret = mixer_.insertChannel(position, channelTypeFromModelTypes(type),
            YADAW::Entity::groupTypeFromConfig(channelConfig),
            channelCount
        );
        if(ret)
        {
            auto& preFaderInserts = mixer_.channelPreFaderInsertsAt(position)->get();
            auto& postFaderInserts = mixer_.channelPostFaderInsertsAt(position)->get();
            preFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
            preFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
            preFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
            postFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
            postFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
            postFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
            beginInsertRows(QModelIndex(), position, position);
            insertModels_.emplace(
                insertModels_.begin() + position,
                std::make_unique<YADAW::Model::MixerChannelInsertListModel>(
                    mixer_.channelPreFaderInsertsAt(position)->get(),
                    listType_,
                    position,
                    true,
                    0
                )
            );
            FOR_RANGE(i, position + 1, insertModels_.size())
            {
                insertModels_[i]->setChannelIndex(i);
            }
            endInsertRows();
        }
        return ret;
    }
    return false;
}

bool MixerChannelListModel::append(IMixerChannelListModel::ChannelTypes type,
    YADAW::Entity::ChannelConfig::Config channelConfig,
    int channelCount)
{
    return insert(itemCount(), type, channelConfig, channelCount);
}

bool MixerChannelListModel::remove(int position, int removeCount)
{
    auto& audioEngine = YADAW::Controller::AudioEngine::appAudioEngine();
    if(position < itemCount() && position + removeCount <= itemCount())
    {
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        FOR_RANGE(i, position, position + removeCount)
        {
            insertModels_[i]->clear();
        }
        (mixer_.*removeChannels[YADAW::Util::underlyingValue(listType_)])(
            position, removeCount
        );
        insertModels_.erase(
            insertModels_.begin() + position,
            insertModels_.begin() + position + removeCount
        );
        FOR_RANGE(i, position, insertModels_.size())
        {
            insertModels_[i]->setChannelIndex(i);
        }
        endRemoveRows();
        return true;
    }
    return false;
}

bool MixerChannelListModel::move(int position, int moveCount, int newPosition)
{
    return false;
}

bool MixerChannelListModel::copy(int position, int copyCount, int newPosition)
{
    return false;
}

void MixerChannelListModel::clear()
{
    remove(0, itemCount());
}
}