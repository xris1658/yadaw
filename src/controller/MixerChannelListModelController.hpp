#ifndef YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
#define YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER

#include "model/HardwareAudioIOPositionModel.hpp"
#include "model/MixerChannelListModel.hpp"
#include "model/MixerChannelSendListModel.hpp"
#include "model/MixerAudioIOPositionItemModel.hpp"
#include "model/RegularAudioIOPositionModel.hpp"

namespace YADAW::Controller
{
struct MixerChannelListModels
{
    YADAW::Model::MixerChannelListModel mixerChannels[3];
    YADAW::Model::HardwareAudioIOPositionModel hardwareAudioInputPositionModel;
    YADAW::Model::HardwareAudioIOPositionModel hardwareAudioOutputPositionModel;
    YADAW::Model::RegularAudioIOPositionModel audioFXIOPositionModel;
    YADAW::Model::RegularAudioIOPositionModel audioGroupIOPositionModel;
    YADAW::Model::MixerAudioIOPositionItemModel audioInputPositionModel;
    YADAW::Model::MixerAudioIOPositionItemModel audioOutputPositionModel;
    MixerChannelListModels(YADAW::Audio::Mixer::Mixer& mixer);
};

MixerChannelListModels& appMixerChannelListModels();

YADAW::Model::MixerChannelListModel& appAudioInputMixerChannels();
YADAW::Model::MixerChannelListModel& appMixerChannels();
YADAW::Model::MixerChannelListModel& appAudioOutputMixerChannels();

YADAW::Model::MixerAudioIOPositionItemModel& appMixerAudioInputPositionModel();
YADAW::Model::MixerAudioIOPositionItemModel& appMixerAudioOutputPositionModel();

void sendInserted(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last);
void sendAboutToBeRemoved(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last);
void sendAboutToBeChanged(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last);
void auxOutputInserted(const YADAW::Model::AuxOutputDestinationModel& sender, int first, int last);
void auxOutputAboutToBeRemoved(const YADAW::Model::AuxOutputDestinationModel& sender, int first, int last);
void auxOutputAboutToBeChanged(const YADAW::Model::AuxOutputDestinationModel& sender, int first, int last);
}

#endif // YADAW_SRC_CONTROLLER_MIXERCHANNELLISTMODELCONTROLLER
