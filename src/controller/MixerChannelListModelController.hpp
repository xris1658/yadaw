#ifndef YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
#define YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER

#include "model/MixerChannelListModel.hpp"
#include "model/MixerChannelSendListModel.hpp"
#include "model/MixerAudioIOPositionItemModel.hpp"

namespace YADAW::Controller
{
YADAW::Model::MixerChannelListModel& appAudioInputMixerChannels();
YADAW::Model::MixerChannelListModel& appMixerChannels();
YADAW::Model::MixerChannelListModel& appAudioOutputMixerChannels();

YADAW::Model::MixerAudioIOPositionItemModel& appMixerAudioInputPositionModel();
YADAW::Model::MixerAudioIOPositionItemModel& appMixerAudioOutputPositionModel();

void sendInserted(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last);
void sendAboutToBeRemoved(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last);
void sendAboutToBeChanged(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last);
}

#endif // YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
