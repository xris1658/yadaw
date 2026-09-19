#include "MixerChannelListModelController.hpp"

#include "controller/AudioEngineController.hpp"

namespace YADAW::Controller
{
YADAW::Entity::IAudioIOPosition* getInputPosition(
    const MixerChannelListModels& models,
    YADAW::Audio::Mixer::Mixer::Position input)
{
    using YADAW::Audio::Mixer::Mixer;
    YADAW::Entity::IAudioIOPosition* ret = nullptr;
    auto& mixer = models.mixer();
    if(input.type == Mixer::Position::Type::AudioHardwareIOChannel)
    {
        auto& model = models.mixerChannels[
            YADAW::Audio::Mixer::Mixer::ChannelListType::AudioHardwareOutputList
        ];
        auto channelIndex = *mixer.getChannelIndexOfId(
            Mixer::ChannelListType::AudioHardwareOutputList, input.id
        );
        ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
            model.data(
                model.index(channelIndex),
                YADAW::Model::MixerChannelListModel::Role::Input
            ).value<QObject*>()
        );
    }
    else if(input.type == Mixer::Position::Type::FXAndGroupChannelInput
        || input.type == Mixer::Position::Type::AudioChannelInput)
    {
        auto& model = models.mixerChannels[
            YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList
        ];
        auto channelIndex = *mixer.getChannelIndexOfId(
            Mixer::ChannelListType::RegularList, input.id
        );
        ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
            model.data(
                model.index(channelIndex),
                YADAW::Model::MixerChannelListModel::Role::Input
            ).value<QObject*>()
        );
    }
    return ret;
}

YADAW::Entity::IAudioIOPosition* getOutputPosition(
    const MixerChannelListModels& models,
    YADAW::Audio::Mixer::Mixer::Position output)
{
    using YADAW::Audio::Mixer::Mixer;
    YADAW::Entity::IAudioIOPosition* ret = nullptr;
    auto& mixer = models.mixer();
    if(output.type == Mixer::Position::Type::AudioHardwareIOChannel)
    {
        auto& model = models.mixerChannels[
            YADAW::Audio::Mixer::Mixer::ChannelListType::AudioHardwareInputList
        ];
        auto channelIndex = *mixer.getChannelIndexOfId(
            Mixer::ChannelListType::AudioHardwareOutputList, output.id
        );
        ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
            model.data(
                model.index(channelIndex),
                YADAW::Model::MixerChannelListModel::Role::Output
            ).value<QObject*>()
        );
    }
    else if(output.type == Mixer::Position::Type::RegularChannelOutput)
    {
        auto& model = models.mixerChannels[
            YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList
        ];
        auto channelIndex = *mixer.getChannelIndexOfId(
            Mixer::ChannelListType::RegularList, output.id
        );
        ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
            model.data(
                model.index(channelIndex),
                YADAW::Model::MixerChannelListModel::Role::Output
            ).value<QObject*>()
        );
    }
    return ret;
}

void mixerMainInputChanged(const YADAW::Audio::Mixer::Mixer& sender,
    std::uint32_t regularChannelIndex)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& models = appMixerChannelListModels();
    auto& model = models.mixerChannels[Mixer::ChannelListType::RegularList];
    auto& output = sender.mainInputAt(regularChannelIndex)->get();
    auto outputAsPosition = getOutputPosition(models, output);
    model.mainInputChanged(regularChannelIndex, outputAsPosition);
}

void mixerMainOutputChanged(const YADAW::Audio::Mixer::Mixer& sender,
    std::uint32_t regularChannelIndex)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& models = appMixerChannelListModels();
    auto& model = models.mixerChannels[Mixer::ChannelListType::RegularList];
    auto& dest = sender.mainOutputAt(regularChannelIndex)->get();
    auto destAsPosition = getInputPosition(models, dest);
    model.mainOutputChanged(regularChannelIndex, destAsPosition);
}
void mixerAuxInputChanged(const YADAW::Audio::Mixer::Mixer& sender,
    const YADAW::Audio::Mixer::Mixer::PluginAuxIOPosition& auxInput)
{
    // TODO
}

void mixerAuxOutputAdded(const YADAW::Audio::Mixer::Mixer& sender,
    YADAW::Audio::Mixer::Mixer::AuxOutputAddedCallbackArgs args)
{
    // TODO
}

void mixerAuxOutputDestinationChanged(const YADAW::Audio::Mixer::Mixer& sender,
    YADAW::Audio::Mixer::Mixer::AuxOutputDestinationChangedCallbackArgs args)
{
    // TODO
}

void mixerAuxOutputRemoved(const YADAW::Audio::Mixer::Mixer& sender,
    YADAW::Audio::Mixer::Mixer::AuxOutputRemovedCallbackArgs args)
{
    // TODO
}

MixerChannelListModels::MixerChannelListModels(YADAW::Audio::Mixer::Mixer& mixer):
    mixerChannels {
        YADAW::Model::MixerChannelListModel(mixer, YADAW::Audio::Mixer::Mixer::ChannelListType::AudioHardwareInputList),
        YADAW::Model::MixerChannelListModel(mixer, YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList),
        YADAW::Model::MixerChannelListModel(mixer, YADAW::Audio::Mixer::Mixer::ChannelListType::AudioHardwareOutputList),
    }
{
    mixer.setMainInputChangedCallback(&mixerMainInputChanged);
    mixer.setMainOutputChangedCallback(&mixerMainOutputChanged);
    mixer.setAuxInputChangedCallback(&mixerAuxInputChanged);
    mixer.setAuxOutputAddedCallback(&mixerAuxOutputAdded);
    mixer.setAuxOutputDestinationChangedCallback(&mixerAuxOutputDestinationChanged);
    mixer.setAuxOutputRemovedCallback(&mixerAuxOutputRemoved);
}

const YADAW::Audio::Mixer::Mixer& MixerChannelListModels::mixer() const
{
    return mixerChannels[0].mixer();
}

YADAW::Audio::Mixer::Mixer& MixerChannelListModels::mixer()
{
    return mixerChannels[0].mixer();
}

MixerChannelListModels& appMixerChannelListModels()
{
    auto& mixer = YADAW::Controller::AudioEngine::appAudioEngine().mixer();
    static MixerChannelListModels ret(mixer);
    return ret;
}
}
