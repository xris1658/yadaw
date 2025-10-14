#include "AuxInputSourceListModel.hpp"

#include "entity/HardwareAudioIOPosition.hpp"
#include "entity/PluginAuxAudioIOPosition.hpp"
#include "entity/RegularAudioIOPosition.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
AuxInputSourceListModel::AuxInputSourceListModel(
    YADAW::Audio::Mixer::Mixer& mixer,
    YADAW::Audio::Mixer::Mixer::ChannelListType channelListType,
    std::uint32_t channelIndex, bool isInstrument,
    bool isPreFaderInsert, std::uint32_t insertIndex, QObject* parent):
    AuxIOTargetListModel(mixer, channelListType, channelIndex, isInstrument,
        isPreFaderInsert, insertIndex, parent)
{
    YADAW::Util::setCppOwnership(*this);
    sources_.resize(
        AuxInputSourceListModel::rowCount({}), nullptr
    );
}

AuxInputSourceListModel::~AuxInputSourceListModel()
{
}

int AuxInputSourceListModel::rowCount(const QModelIndex& parent) const
{
    if(channelListType_ != YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList
        || !isInstrument_)
    {
        auto type = static_cast<YADAW::Audio::Mixer::Mixer::ChannelListType>(channelListType_);
        auto& inserts = (isPreFaderInsert_?
            mixer_->preFaderInsertsAt(type, channelIndex_):
            mixer_->postFaderInsertsAt(type, channelIndex_))->get();
        auto device = mixer_->graph().graph().getNodeData(
            *(inserts.insertNodeAt(insertIndex_))
        ).process.device();
        return device->audioInputGroupCount() - 1;
    }
    auto device = mixer_->graph().graph().getNodeData(
        mixer_->getInstrument(channelIndex_)
    ).process.device();
    return device->audioInputGroupCount();
}

QVariant AuxInputSourceListModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0 && row < rowCount(QModelIndex()))
    {
        if(role == Role::ChannelIndex)
        {
            if(channelListType_ != YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList
                || !isInstrument_)
            {
                auto type = static_cast<YADAW::Audio::Mixer::Mixer::ChannelListType>(channelListType_);
                auto& inserts = (isPreFaderInsert_?
                    mixer_->preFaderInsertsAt(type, channelIndex_):
                    mixer_->postFaderInsertsAt(type, channelIndex_))->get();
                auto mainIndex = *inserts.insertInputChannelGroupIndexAt(insertIndex_);
                return row + (row >= mainIndex);
            }
            return row;
        }
        else if(role == Role::Target)
        {
            return QVariant::fromValue<QObject*>(
                const_cast<YADAW::Entity::IAudioIOPosition*>(sources_[row])
            );
        }
    }
    return {};
}

bool AuxInputSourceListModel::setData(const QModelIndex& index,
    const QVariant& value, int role)
{
    using YADAW::Audio::Mixer::Mixer;
    using YADAW::Entity::IAudioIOPosition;
    using YADAW::Entity::HardwareAudioIOPosition;
    using YADAW::Entity::PluginAuxAudioIOPosition;
    using YADAW::Entity::RegularAudioIOPosition;
    auto ret = false;
    auto row = index.row();
    if(row >= 0 && row < rowCount(QModelIndex()) && role == Role::Target)
    {
        auto obj = value.value<QObject*>();
        if(auto pPosition = dynamic_cast<IAudioIOPosition*>(obj))
        {
            Mixer::Position source;
            auto type = pPosition->getType();
            if(type == IAudioIOPosition::Type::AudioHardwareIOChannel)
            {
                auto audioHardwareIOPosition = static_cast<HardwareAudioIOPosition*>(pPosition);
                source = static_cast<Mixer::Position>(*audioHardwareIOPosition);
            }
            else if(type == IAudioIOPosition::Type::BusAndFXChannel)
            {
                auto busAndFXPosition = static_cast<RegularAudioIOPosition*>(pPosition);
                source = static_cast<Mixer::Position>(*busAndFXPosition);
            }
            else if(type == IAudioIOPosition::Type::PluginAuxIO)
            {
                auto pluginAuxPosition = static_cast<PluginAuxAudioIOPosition*>(pPosition);
                source = static_cast<Mixer::Position>(*pluginAuxPosition);
            }
            if(source.type != Mixer::Position::Type::Invalid)
            {
                auto channelGroupIndex = data(index, Role::ChannelIndex);
                if(channelGroupIndex.isValid())
                {
                    auto self = position(channelGroupIndex.value<int>());
                    ret = mixer_->setAuxInputSource(self, source);
                }
            }
        }
    }
    if(ret)
    {
        dataChanged(index, index, {Role::Target});
    }
    return ret;
}

void AuxInputSourceListModel::inputChanged(
    std::uint32_t index, const YADAW::Entity::IAudioIOPosition* position)
{
    if(index < sources_.size())
    {
        auto oldPosition = sources_[index];
        sources_[index] = const_cast<YADAW::Entity::IAudioIOPosition*>(position); // FIXME?
        dataChanged(this->index(index), this->index(index), {Role::Target});
    }
}
}
