#include "MixerChannelListModelController.hpp"

#include "controller/AudioEngineController.hpp"

namespace YADAW::Controller
{
YADAW::Model::MixerChannelListModel& appAudioInputMixerChannels()
{
    static YADAW::Model::MixerChannelListModel ret(
        YADAW::Controller::AudioEngine::appAudioEngine().mixer(),
        YADAW::Model::MixerChannelListModel::ListType::AudioHardwareInput
    );
    return ret;
}

YADAW::Model::MixerChannelListModel& appMixerChannels()
{
    static YADAW::Model::MixerChannelListModel ret(
        YADAW::Controller::AudioEngine::appAudioEngine().mixer(),
        YADAW::Model::MixerChannelListModel::ListType::Regular
    );
    return ret;
}

YADAW::Model::MixerChannelListModel& appAudioOutputMixerChannels()
{
    static YADAW::Model::MixerChannelListModel ret(
        YADAW::Controller::AudioEngine::appAudioEngine().mixer(),
        YADAW::Model::MixerChannelListModel::ListType::AudioHardwareOutput
    );
    return ret;
}
}