#include "AuxIOTargetListModel.hpp"

#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
AuxIOTargetListModel::AuxIOTargetListModel(
    YADAW::Audio::Mixer::Mixer& mixer,
    YADAW::Audio::Mixer::Mixer::ChannelListType channelListType, std::uint32_t channelIndex, bool isInstrument,
    bool isPreFaderInsert, std::uint32_t insertIndex,
    QObject* parent):
    IAuxIOTargetListModel(parent),
    mixer_(&mixer), channelListType_(channelListType), isInstrument_(isInstrument),
    isPreFaderInsert_(isPreFaderInsert),
    channelIndex_(channelIndex), insertIndex_(insertIndex)
{
    YADAW::Util::setCppOwnership(*this);
}

AuxIOTargetListModel::~AuxIOTargetListModel()
{}

YADAW::Audio::Mixer::Mixer::PluginAuxIOPosition AuxIOTargetListModel::position(std::uint32_t channelGroupIndex) const
{
    using YADAW::Audio::Mixer::Mixer;
    return Mixer::PluginAuxIOPosition {
        .channelListType = static_cast<Mixer::ChannelListType>(channelListType_),
        .channelIndex = channelIndex_,
        .inChannelPosition = isInstrument_?
            Mixer::PluginAuxIOPosition::InChannelPosition::Instrument:
            Mixer::PluginAuxIOPosition::InChannelPosition::Inserts,
        .isPreFaderInsert = static_cast<bool>(isPreFaderInsert_),
        .insertIndex = insertIndex_,
        .channelGroupIndex = channelGroupIndex
    };
}
}
