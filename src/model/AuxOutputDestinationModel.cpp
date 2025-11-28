#include "AuxOutputDestinationModel.hpp"

#include "entity/HardwareAudioIOPosition.hpp"
#include "entity/PluginAuxAudioIOPosition.hpp"
#include "entity/RegularAudioIOPosition.hpp"
#include "model/AuxOutputDestinationListModel.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
AuxOutputDestinationModel::AuxOutputDestinationModel(
    YADAW::Model::AuxOutputDestinationListModel& model,
    std::uint32_t channelGroupIndex, QObject* parent):
    IAuxOutputDestinationListModel(parent),
    model_(&model), channelGroupIndex_(channelGroupIndex)
{
    YADAW::Util::setCppOwnership(*this);
    QObject::connect(
        this, &AuxOutputDestinationModel::added,
        this, &AuxOutputDestinationModel::onAdded
    );
    QObject::connect(
        this, &AuxOutputDestinationModel::destinationChanged,
        this, &AuxOutputDestinationModel::onDestinationChanged
    );
    QObject::connect(
        this, &AuxOutputDestinationModel::removed,
        this, &AuxOutputDestinationModel::onRemoved
    );
}

AuxOutputDestinationModel::~AuxOutputDestinationModel()
{
}

YADAW::Model::AuxOutputDestinationListModel& AuxOutputDestinationModel::getModel() const
{
    return *model_;
}

std::uint32_t AuxOutputDestinationModel::getChannelGroupIndex() const
{
    return channelGroupIndex_;
}

int AuxOutputDestinationModel::rowCount(const QModelIndex& parent) const
{
    return positions_.size();
}

QVariant AuxOutputDestinationModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0 && row < positions_.size() && role == Role::Destination)
    {
        return QVariant::fromValue<QObject*>(
            const_cast<YADAW::Entity::IAudioIOPosition*>(positions_[row])
        );
    }
    return {};
}

bool AuxOutputDestinationModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    using YADAW::Audio::Mixer::Mixer;
    using YADAW::Entity::IAudioIOPosition;
    using YADAW::Entity::HardwareAudioIOPosition;
    using YADAW::Entity::PluginAuxAudioIOPosition;
    using YADAW::Entity::RegularAudioIOPosition;

    if(auto row = index.row(); row >= 0 && row < positions_.size())
    {
        auto obj = value.value<QObject*>();
        if(auto pPosition = dynamic_cast<IAudioIOPosition*>(obj))
        {
            auto self = model_->position(channelGroupIndex_);
            Mixer::Position dest;
            auto type = pPosition->getType();
            if(type == IAudioIOPosition::Type::AudioHardwareIOChannel)
            {
                auto audioHardwareIOPosition = static_cast<HardwareAudioIOPosition*>(pPosition);
                dest = static_cast<Mixer::Position>(*audioHardwareIOPosition);
            }
            else if(type == IAudioIOPosition::Type::BusAndFXChannel)
            {
                auto busAndFXPosition = static_cast<RegularAudioIOPosition*>(pPosition);
                dest = static_cast<Mixer::Position>(*busAndFXPosition);
            }
            else if(type == IAudioIOPosition::Type::PluginAuxIO)
            {
                auto pluginAuxPosition = static_cast<PluginAuxAudioIOPosition*>(pPosition);
                dest = static_cast<Mixer::Position>(*pluginAuxPosition);
            }
            if(dest.type != Mixer::Position::Type::Invalid)
            {
                return model_->mixer_->setAuxOutputDestination(self, row, dest);
            }
        }
    }
    return false;
}

bool AuxOutputDestinationModel::append(YADAW::Entity::IAudioIOPosition* position)
{
    YADAW::Audio::Mixer::Mixer::Position dest;
    if(auto type = position->getType(); type == YADAW::Entity::IAudioIOPosition::Type::AudioHardwareIOChannel)
    {
        dest = static_cast<YADAW::Audio::Mixer::Mixer::Position>(
            static_cast<YADAW::Entity::HardwareAudioIOPosition&>(*position)
        );
    }
    else if(type == YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel)
    {
        dest = static_cast<YADAW::Audio::Mixer::Mixer::Position>(
            static_cast<YADAW::Entity::RegularAudioIOPosition&>(*position)
        );
    }
    else if(type == YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO)
    {
        dest = static_cast<YADAW::Audio::Mixer::Mixer::Position>(
            static_cast<YADAW::Entity::PluginAuxAudioIOPosition&>(*position)
        );
    }
    if(dest.type != YADAW::Audio::Mixer::Mixer::Position::Type::Invalid)
    {
        return model_->mixer_->addAuxOutputDestination(
            model_->position(channelGroupIndex_), dest
        );
    }
    return false;
}

bool AuxOutputDestinationModel::remove(int position, int removeCount)
{
    if(removeCount > 0 && position >= 0 && position < positions_.size() && position + removeCount <= positions_.size())
    {
        auto self = model_->position(channelGroupIndex_);
        if(model_->mixer_->removeAuxOutputDestination(self, position, removeCount))
        {
            beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
            positions_.erase(positions_.begin() + position, positions_.begin() + position + removeCount);
            endRemoveRows();
            return true;
        }
    }
    return false;
}

void AuxOutputDestinationModel::onAdded(int index, YADAW::Entity::IAudioIOPosition* position)
{
    if(index >= 0 && index <= positions_.size())
    {
        beginInsertRows(QModelIndex(), index, index);
        positions_.emplace(positions_.begin() + index, position);
        endInsertRows();
    }
}

void AuxOutputDestinationModel::onDestinationChanged(
    int index, YADAW::Entity::IAudioIOPosition* position)
{
    if(index >= 0 && index <= positions_.size())
    {
        positions_[index] = position;
        dataChanged(this->index(index), this->index(index), {Role::Destination});
    }
}

void AuxOutputDestinationModel::onRemoved(int first, int last)
{
    if(first >= 0 && last < positions_.size() && first <= last)
    {
        beginRemoveRows(QModelIndex(), first, last);
        positions_.erase(positions_.begin() + first, positions_.begin() + last + 1);
        endRemoveRows();
    }
}
}
