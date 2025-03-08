#include "ConnectInsertToAudioIOPosition.hpp"

namespace YADAW::Model::Impl
{
ConnectInsertToAudioIOPosition::ConnectInsertToAudioIOPosition(
    MixerChannelInsertListModel& from, MixerAudioIOPositionItemModel& to,
    const QModelIndex& index):
    from_(&from), to_(&to), index_(index)
{
}

void ConnectInsertToAudioIOPosition::updateIndex(const QModelIndex& index)
{
    index_ = index;
}
}
