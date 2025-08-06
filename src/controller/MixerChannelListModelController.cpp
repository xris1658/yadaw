#include "MixerChannelListModelController.hpp"

#include "controller/AudioEngineController.hpp"

namespace YADAW::Controller
{
YADAW::Model::MixerChannelListModel& appAudioInputMixerChannels()
{
    static YADAW::Model::MixerChannelListModel ret(
        YADAW::Controller::AudioEngine::appAudioEngine().mixer(),
        YADAW::Audio::Mixer::Mixer::ChannelListType::AudioHardwareInputList
    );
    return ret;
}

YADAW::Model::MixerChannelListModel& appMixerChannels()
{
    static YADAW::Model::MixerChannelListModel ret(
        YADAW::Controller::AudioEngine::appAudioEngine().mixer(),
        YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList
    );
    return ret;
}

YADAW::Model::MixerChannelListModel& appAudioOutputMixerChannels()
{
    static YADAW::Model::MixerChannelListModel ret(
        YADAW::Controller::AudioEngine::appAudioEngine().mixer(),
        YADAW::Audio::Mixer::Mixer::ChannelListType::AudioHardwareOutputList
    );
    return ret;
}

YADAW::Model::MixerAudioIOPositionItemModel& appMixerAudioInputPositionModel()
{
    static YADAW::Model::MixerAudioIOPositionItemModel ret(
        appAudioInputMixerChannels(),
        appMixerChannels(),
        appAudioOutputMixerChannels(),
        true
    );
    return ret;
}

YADAW::Model::MixerAudioIOPositionItemModel& appMixerAudioOutputPositionModel()
{
    static YADAW::Model::MixerAudioIOPositionItemModel ret(
        appAudioInputMixerChannels(),
        appMixerChannels(),
        appAudioOutputMixerChannels(),
        false
    );
    return ret;
}
}
