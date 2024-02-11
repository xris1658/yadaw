#ifndef YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
#define YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER

#include "model/MixerChannelListModel.hpp"

namespace YADAW::Controller
{
YADAW::Model::MixerChannelListModel& appAudioInputMixerChannels();
YADAW::Model::MixerChannelListModel& appMixerChannels();
YADAW::Model::MixerChannelListModel& appAudioOutputMixerChannels();
}

#endif // YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
