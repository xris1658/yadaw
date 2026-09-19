#ifndef YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
#define YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER

#include "model/MixerChannelListModel.hpp"

namespace YADAW::Controller
{
struct MixerChannelListModels
{
    YADAW::Model::MixerChannelListModel mixerChannels[3];
    MixerChannelListModels(YADAW::Audio::Mixer::Mixer& mixer);
    const YADAW::Audio::Mixer::Mixer& mixer() const;
          YADAW::Audio::Mixer::Mixer& mixer();
};

MixerChannelListModels& appMixerChannelListModels();
}

#endif // YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
