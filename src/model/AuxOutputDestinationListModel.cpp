#include "AuxOutputDestinationListModel.hpp"

#include "model/AuxOutputDestinationModel.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
AuxOutputDestinationListModel::AuxOutputDestinationListModel(
    YADAW::Audio::Mixer::Mixer& mixer,
    YADAW::Audio::Mixer::Mixer::ChannelListType channelListType,
    std::uint32_t channelIndex, bool isInstrument,
    bool isPreFaderInsert, std::uint32_t insertIndex, QObject* parent):
    AuxIOTargetListModel(mixer, channelListType, channelIndex, isInstrument,
        isPreFaderInsert, insertIndex, parent)
{
    YADAW::Util::setCppOwnership(*this);
    auto size = AuxOutputDestinationListModel::rowCount({});
    destinations_.reserve(size);
    FOR_RANGE0(i, size)
    {
        auto channelGroupIndex = AuxOutputDestinationListModel::data(
            AuxOutputDestinationListModel::index(i), Role::ChannelIndex
        ).value<int>();
        destinations_.emplace_back(
            std::make_unique<YADAW::Model::AuxOutputDestinationModel>(*this, channelGroupIndex)
        );
    }
}

AuxOutputDestinationListModel::~AuxOutputDestinationListModel()
{
}

int AuxOutputDestinationListModel::rowCount(const QModelIndex& parent) const
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
        return device->audioOutputGroupCount() - 1;
    }
    auto device = mixer_->graph().graph().getNodeData(
        mixer_->getInstrument(channelIndex_)
    ).process.device();
    return device->audioOutputGroupCount() - 1;
}

QVariant AuxOutputDestinationListModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0 && row < rowCount(QModelIndex()))
    {
        if(role == Role::ChannelIndex)
        {
            if(auto type = static_cast<YADAW::Audio::Mixer::Mixer::ChannelListType>(channelListType_);
                type != YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList
                || !isInstrument_)
            {
                auto& inserts = (isPreFaderInsert_?
                    mixer_->preFaderInsertsAt(type, channelIndex_):
                    mixer_->postFaderInsertsAt(type, channelIndex_))->get();
                auto mainIndex = *inserts.insertInputChannelGroupIndexAt(insertIndex_);
                return row + (row >= mainIndex);
            }
            else
            {
                auto mainIndex = *(mixer_->getInstrumentMainOutputChannelGroupIndex(channelIndex_));
                return row + (row >= mainIndex);
            }
        }
        else if(role == Role::Target)
        {
            return QVariant::fromValue<QObject*>(
                const_cast<YADAW::Model::AuxOutputDestinationModel*>(
                    destinations_[row].get()
                )
            );
        }
    }
    return {};
}
}
