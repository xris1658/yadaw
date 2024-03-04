#include "AudioBusConfigurationModel.hpp"

#include "entity/ChannelConfigHelper.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
AudioBusConfigurationModel::AudioBusConfigurationModel(
    YADAW::Audio::Device::IAudioBusConfiguration& configuration,
    bool isInput):
    configuration_(&configuration),
    channelList_(),
    name_(),
    isInput_(isInput)
{
    auto itemCount = this->itemCount();
    name_.resize(itemCount);
    channelListConnections_.reserve(itemCount);
    FOR_RANGE0(i, itemCount)
    {
        channelList_.emplace_back(configuration, *this, isInput_, i);
        channelListConnections_.emplace_back(
            QObject::connect(
                &channelList_[i],
                &YADAW::Model::AudioBusChannelListModel::dataChanged,
                [this, i](const QModelIndex&, const QModelIndex&, const QList<int>&)
                {
                    dataChanged(this->index(i), this->index(i),
                        {Role::ChannelList}
                    );
                }
            )
        );
    }
}

AudioBusConfigurationModel::~AudioBusConfigurationModel()
{}

std::uint32_t AudioBusConfigurationModel::itemCount() const
{
    return isInput_? configuration_->inputBusCount():
        configuration_->outputBusCount();
}

int AudioBusConfigurationModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant AudioBusConfigurationModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
            return QVariant::fromValue(name_[row]);
        case Role::ChannelConfig:
            return QVariant::fromValue<int>(
                static_cast<int>(
                    (isInput_?
                        configuration_->inputBusAt(row):
                        configuration_->outputBusAt(row)
                    )->get().channelGroupType()
                ) + 1
            );
        case Role::ChannelList:
            // Some crappy code right here
            return QVariant::fromValue<QObject*>(
                const_cast<YADAW::Model::AudioBusChannelListModel*>(
                    &channelList_[row]
                )
            );
        }
    }
    return {};
}

bool AudioBusConfigurationModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
            name_[row] = value.value<QString>();
            dataChanged(this->index(row), this->index(row), {Role::Name});
            return true;
        }
    }
    return false;
}

bool AudioBusConfigurationModel::append(int channelConfig)
{
    return insert(itemCount(), channelConfig);
}

bool AudioBusConfigurationModel::insert(int position, int channelConfig)
{
    if(position >= 0 && position <= itemCount())
    {
        auto ret = configuration_->insertBus(
            position, isInput_,
            YADAW::Entity::groupTypeFromConfig(
                static_cast<YADAW::Entity::ChannelConfig::Config>(channelConfig)
            )
        );
        if(ret)
        {
            beginInsertRows(QModelIndex(), position, position);
            auto channelList = &*channelList_.emplace(
                channelList_.begin() + position,
                *configuration_, *this, isInput_, position
            );
            auto connection = QObject::connect(
                channelList,
                &YADAW::Model::AudioBusChannelListModel::dataChanged,
                [this, position](
                    const QModelIndex&,
                    const QModelIndex&,
                    const QList<int>&)
                {
                    dataChanged(
                        this->index(position),
                        this->index(position),
                        {IAudioBusConfigurationModel::Role::ChannelList}
                    );
                }
            );
            channelListConnections_.emplace(
                channelListConnections_.begin() + position,
                connection
            );
            FOR_RANGE(i, position + 1, channelListConnections_.size())
            {
                QObject::disconnect(channelListConnections_[i]);
                channelListConnections_[i] = QObject::connect(
                    &channelList_[i],
                    &YADAW::Model::AudioBusChannelListModel::dataChanged,
                    [this, i](
                        const QModelIndex&,
                        const QModelIndex&,
                        const QList<int>&)
                    {
                        dataChanged(
                            this->index(i),
                            this->index(i),
                            {IAudioBusConfigurationModel::Role::ChannelList}
                        );
                    }
                );
            }
            name_.emplace(name_.begin() + position);
            endInsertRows();
        }
        return ret;
    }
}

bool AudioBusConfigurationModel::remove(int index)
{
    if(index >= 0 && index < itemCount())
    {
        beginRemoveRows(QModelIndex(), index, index);
        name_.erase(name_.begin() + index);
        configuration_->removeBus(isInput_, index);
        channelList_.clear();
        for(std::uint32_t i = 0; i < itemCount(); ++i)
        {
            channelList_.emplace_back(*configuration_, *this, isInput_, i);
        }
        channelListConnections_.erase(channelListConnections_.begin() + index);
        FOR_RANGE(i, index, channelListConnections_.size())
        {
            QObject::disconnect(channelListConnections_[i]);
            channelListConnections_[i] = QObject::connect(
                &channelList_[i],
                &YADAW::Model::AudioBusChannelListModel::dataChanged,
                [this, i](
                    const QModelIndex&,
                    const QModelIndex&,
                    const QList<int>&)
                {
                    dataChanged(
                        this->index(i),
                        this->index(i),
                        {IAudioBusConfigurationModel::Role::ChannelList}
                    );
                }
            );
        }
        endRemoveRows();
        return true;
    }
    return false;
}

void AudioBusConfigurationModel::clear()
{
    configuration_->clearBus(isInput_);
}

OptionalRef<const QString> AudioBusConfigurationModel::nameAt(std::uint32_t index) const
{
    auto count = itemCount();
    return index < count? OptionalRef<const QString>(name_[index]): std::nullopt;
}

bool AudioBusConfigurationModel::setName(std::uint32_t index, const QString& name)
{
    if(index < itemCount())
    {
        name_[index] = name;
        return true;
    }
    return false;
}

bool AudioBusConfigurationModel::setChannel(
    std::uint32_t index, std::uint32_t busChannelIndex,
    std::uint32_t deviceIndex, std::uint32_t deviceChannelIndex)
{
    if(index < channelList_.size())
    {
        return channelList_[index].setChannel(busChannelIndex, deviceIndex, deviceChannelIndex);
    }
    return false;
}
}