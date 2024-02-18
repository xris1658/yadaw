#include "MixerChannelListModel.hpp"

#include "entity/ChannelConfigHelper.hpp"
#include "util/Base.hpp"

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
                (mixer_.*getPreFaderInserts[YADAW::Util::underlyingValue(listType_)])(i)->get()
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
            beginInsertRows(QModelIndex(), position, position);
            insertModels_.emplace_back(
                std::make_unique<YADAW::Model::MixerChannelInsertListModel>(
                    mixer_.channelPreFaderInsertsAt(position)->get()
                )
            );
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
    if(position < itemCount() && position + removeCount <= itemCount())
    {
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        (mixer_.*removeChannels[YADAW::Util::underlyingValue(listType_)])(
            position, removeCount
        );
        insertModels_.erase(
            insertModels_.begin() + position,
            insertModels_.begin() + position + removeCount
        );
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