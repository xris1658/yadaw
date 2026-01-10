#ifndef YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
#define YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER

#include "model/MixerChannelListModel.hpp"
#include "model/MixerChannelSendListModel.hpp"
#include "model/MixerAudioIOPositionItemModel.hpp"

namespace YADAW::Controller
{
struct MixerChannelListModels
{
    YADAW::Model::MixerChannelListModel mixerChannels[3];
    YADAW::Model::MixerAudioIOPositionItemModel audioInputPositionModel;
    YADAW::Model::MixerAudioIOPositionItemModel audioOutputPositionModel;
    MixerChannelListModels(YADAW::Audio::Mixer::Mixer& mixer);
    const YADAW::Audio::Mixer::Mixer& mixer() const;
          YADAW::Audio::Mixer::Mixer& mixer();
};

MixerChannelListModels& appMixerChannelListModels();
}

#endif // YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
