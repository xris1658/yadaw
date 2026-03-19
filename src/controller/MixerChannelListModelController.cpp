#include "MixerChannelListModelController.hpp"

#include "controller/AudioEngineController.hpp"
#include "model/MixerChannelInsertListModel.hpp"

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
    else if(input.type == Mixer::Position::Type::PluginAuxIO)
    {
        ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
            models.audioInputPositionModel.data(
                models.audioInputPositionModel.findIndexByID(QString::number(input.id)),
                YADAW::Model::MixerAudioIOPositionItemModel::Role::Position
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
    else if(output.type == Mixer::Position::Type::PluginAuxIO)
    {
        ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
            models.audioOutputPositionModel.data(
                models.audioOutputPositionModel.findIndexByID(QString::number(output.id)),
                YADAW::Model::MixerAudioIOPositionItemModel::Role::Position
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
    using YADAW::Audio::Mixer::Mixer;
    auto& models = appMixerChannelListModels();
    auto output = *sender.getAuxInputSource(auxInput);
    auto outputAsPosition = getOutputPosition(models, output);
    auto& model = models.mixerChannels[auxInput.channelListType];
    YADAW::Model::AuxInputSourceListModel* auxInputSourceListModel = nullptr;
    std::uint32_t modelRowIndex = 0;
    if(auxInput.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Instrument)
    {
        auxInputSourceListModel = static_cast<YADAW::Model::AuxInputSourceListModel*>(
            model.data(
                model.index(auxInput.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::InstrumentAudioAuxInputSource
            ).value<QObject*>()
        );
        modelRowIndex = auxInput.channelGroupIndex;
    }
    else
    {
        auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(
            model.data(model.index(auxInput.channelIndex), YADAW::Model::MixerChannelListModel::Role::Inserts).value<QObject*>()
        );
        auxInputSourceListModel = static_cast<YADAW::Model::AuxInputSourceListModel*>(
            insertListModel->data(
                insertListModel->index(auxInput.insertIndex), YADAW::Model::MixerChannelInsertListModel::Role::AudioAuxInputSource
            ).value<QObject*>()
        );
        modelRowIndex = auxInput.channelGroupIndex - (
            auxInput.channelGroupIndex > insertListModel->inserts().insertInputChannelGroupIndexAt(auxInput.insertIndex)
        );
    }
    auxInputSourceListModel->inputChanged(modelRowIndex, outputAsPosition);
}

void mixerAuxOutputAdded(const YADAW::Audio::Mixer::Mixer& sender,
    YADAW::Audio::Mixer::Mixer::AuxOutputAddedCallbackArgs args)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& destinations = sender.getAuxOutputDestinations(args.auxOutput)->get();
    assert(
        destinations.size() - 1 == args.position
    );
    const auto& models = appMixerChannelListModels();
    auto& model = models.mixerChannels[args.auxOutput.channelListType];
    YADAW::Model::AuxOutputDestinationModel* auxOutputDestination = nullptr;
    if(args.auxOutput.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Instrument)
    {
        auto auxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationListModel*>(
            model.data(
                model.index(args.auxOutput.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::InstrumentAudioAuxOutputDestination
            ).value<QObject*>()
        );
        auxOutputDestination = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            auxOutputDestinationList->data(
                auxOutputDestinationList->index(
                    args.auxOutput.channelGroupIndex - (
                        args.auxOutput.channelGroupIndex > sender.getInstrumentMainOutputChannelGroupIndex(args.auxOutput.channelIndex)
                    )
                ),
                YADAW::Model::AuxOutputDestinationListModel::Role::Target
            ).value<QObject*>()
        );
    }
    else
    {
        auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(
            model.data(
                model.index(args.auxOutput.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::Inserts
            ).value<QObject*>()
        );
        auto auxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            insertListModel->data(
                insertListModel->index(args.auxOutput.insertIndex),
                YADAW::Model::MixerChannelInsertListModel::Role::AudioAuxOutputDestination
            ).value<QObject*>()
        );
        auxOutputDestination = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            auxOutputDestinationList->data(
                auxOutputDestinationList->index(
                    args.auxOutput.channelGroupIndex - (
                        args.auxOutput.channelGroupIndex > insertListModel->inserts().insertOutputChannelGroupIndexAt(args.auxOutput.insertIndex)
                    )
                ),
                YADAW::Model::AuxOutputDestinationListModel::Role::Target
            ).value<QObject*>()
        );
    }
    assert(auxOutputDestination);
    auto& dest = destinations.back();
    auto destAsPosition = getInputPosition(models, dest);
    assert(destAsPosition);
    auxOutputDestination->added(destinations.size() - 1, destAsPosition);
}

void mixerAuxOutputDestinationChanged(const YADAW::Audio::Mixer::Mixer& sender,
    YADAW::Audio::Mixer::Mixer::AuxOutputDestinationChangedCallbackArgs args)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& destinations = sender.getAuxOutputDestinations(args.auxOutput)->get();
    assert(
        destinations.size() - 1 == args.position
    );
    const auto& models = appMixerChannelListModels();
    auto& model = models.mixerChannels[args.auxOutput.channelListType];
    YADAW::Model::AuxOutputDestinationModel* auxOutputDestination = nullptr;
    if(args.auxOutput.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Instrument)
    {
        auto auxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationListModel*>(
            model.data(
                model.index(args.auxOutput.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::InstrumentAudioAuxOutputDestination
            ).value<QObject*>()
        );
        auxOutputDestination = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            auxOutputDestinationList->data(
                auxOutputDestinationList->index(
                    args.auxOutput.channelGroupIndex - (
                        args.auxOutput.channelGroupIndex > sender.getInstrumentMainOutputChannelGroupIndex(args.auxOutput.channelIndex)
                    )
                ),
                YADAW::Model::AuxOutputDestinationListModel::Role::Target
            ).value<QObject*>()
        );
    }
    else
    {
        auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(
            model.data(
                model.index(args.auxOutput.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::Inserts
            ).value<QObject*>()
        );
        auto auxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            insertListModel->data(
                insertListModel->index(args.auxOutput.insertIndex),
                YADAW::Model::MixerChannelInsertListModel::Role::AudioAuxOutputDestination
            ).value<QObject*>()
        );
        auxOutputDestination = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            auxOutputDestinationList->data(
                auxOutputDestinationList->index(
                    args.auxOutput.channelGroupIndex - (
                        args.auxOutput.channelGroupIndex > insertListModel->inserts().insertOutputChannelGroupIndexAt(args.auxOutput.insertIndex)
                    )
                ),
                YADAW::Model::AuxOutputDestinationListModel::Role::Target
            ).value<QObject*>()
        );
    }
    assert(auxOutputDestination);
    auto& dest = destinations[args.position];
    auto destAsPosition = getInputPosition(models, dest);
    assert(destAsPosition);
    auxOutputDestination->destinationChanged(args.position, destAsPosition);
}

void mixerAuxOutputRemoved(const YADAW::Audio::Mixer::Mixer& sender,
    YADAW::Audio::Mixer::Mixer::AuxOutputRemovedCallbackArgs args)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& destinations = sender.getAuxOutputDestinations(args.auxOutput)->get();
    auto& models = appMixerChannelListModels();
    auto& model = models.mixerChannels[args.auxOutput.channelListType];
    YADAW::Model::AuxOutputDestinationModel* auxOutputDestination = nullptr;
    if(args.auxOutput.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Instrument)
    {
        auto auxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationListModel*>(
            model.data(
                model.index(args.auxOutput.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::InstrumentAudioAuxOutputDestination
            ).value<QObject*>()
        );
        auxOutputDestination = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            auxOutputDestinationList->data(
                auxOutputDestinationList->index(
                    args.auxOutput.channelGroupIndex - (
                        args.auxOutput.channelGroupIndex > sender.getInstrumentMainOutputChannelGroupIndex(args.auxOutput.channelIndex)
                    )
                ),
                YADAW::Model::AuxOutputDestinationListModel::Role::Target
            ).value<QObject*>()
        );
    }
    else
    {
        auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(
            model.data(
                model.index(args.auxOutput.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::Inserts
            ).value<QObject*>()
        );
        auto auxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            insertListModel->data(
                insertListModel->index(args.auxOutput.insertIndex),
                YADAW::Model::MixerChannelInsertListModel::Role::AudioAuxOutputDestination
            ).value<QObject*>()
        );
        auxOutputDestination = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            auxOutputDestinationList->data(
                auxOutputDestinationList->index(
                    args.auxOutput.channelGroupIndex - (
                        args.auxOutput.channelGroupIndex > insertListModel->inserts().insertOutputChannelGroupIndexAt(args.auxOutput.insertIndex)
                    )
                ),
                YADAW::Model::AuxOutputDestinationListModel::Role::Target
            ).value<QObject*>()
        );
    }
    assert(auxOutputDestination);
    auxOutputDestination->removed(args.position, args.position + args.removeCount - 1);
}

MixerChannelListModels::MixerChannelListModels(YADAW::Audio::Mixer::Mixer& mixer):
    mixerChannels {
        YADAW::Model::MixerChannelListModel(mixer, YADAW::Audio::Mixer::Mixer::ChannelListType::AudioHardwareInputList),
        YADAW::Model::MixerChannelListModel(mixer, YADAW::Audio::Mixer::Mixer::ChannelListType::RegularList),
        YADAW::Model::MixerChannelListModel(mixer, YADAW::Audio::Mixer::Mixer::ChannelListType::AudioHardwareOutputList),
    },
    audioInputPositionModel(mixerChannels[0], mixerChannels[1], mixerChannels[2], true),
    audioOutputPositionModel(mixerChannels[0], mixerChannels[1], mixerChannels[2], false)
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
