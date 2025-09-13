#include "AuxIOTargetListModel.hpp"

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
{}

AuxIOTargetListModel::~AuxIOTargetListModel()
{}

// TODO
}
