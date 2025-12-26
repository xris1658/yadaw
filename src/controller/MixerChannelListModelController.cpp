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
        auto channelIndex = *mixer.getChannelIndexOfId(
            Mixer::ChannelListType::AudioHardwareOutputList, input.id
        );
        ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
            models.hardwareAudioOutputPositionModel.data(
                models.hardwareAudioOutputPositionModel.index(channelIndex),
                YADAW::Model::HardwareAudioIOPositionModel::Role::Position
            ).value<QObject*>()
        );
    }
    else if(input.type == Mixer::Position::Type::FXAndGroupChannelInput)
    {
        auto channelIndex = *mixer.getChannelIndexOfId(
            Mixer::ChannelListType::RegularList, input.id
        );
        if(auto channelType = mixer.channelInfoAt(
                Mixer::ChannelListType::RegularList, channelIndex
            )->get().channelType;
            channelType == Mixer::ChannelType::AudioFX)
        {
            ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
                models.audioFXIOPositionModel.data(
                    models.audioFXIOPositionModel.index(
                        models.audioFXIOPositionModel.findIndexByID(QString::number(input.id))
                    ),
                    YADAW::Model::RegularAudioIOPositionModel::Role::Position
                ).value<QObject*>()
            );
        }
        else if(channelType == Mixer::ChannelType::AudioBus)
        {
            ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
                models.audioGroupIOPositionModel.data(
                    models.audioGroupIOPositionModel.index(
                        models.audioGroupIOPositionModel.findIndexByID(QString::number(input.id))
                    ),
                    YADAW::Model::RegularAudioIOPositionModel::Role::Position
                ).value<QObject*>()
            );
        }
    }
    else if(input.type == Mixer::Position::Type::AudioChannelInput)
    {
        auto channelIndex = *mixer.getChannelIndexOfId(
            Mixer::ChannelListType::RegularList, input.id
        );
        if(auto channelType = mixer.channelInfoAt(
                Mixer::ChannelListType::RegularList, channelIndex
            )->get().channelType;
            channelType == Mixer::ChannelType::Audio)
        {
            ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
                models.audioFXIOPositionModel.data(
                    models.audioFXIOPositionModel.index(
                        models.audioFXIOPositionModel.findIndexByID(QString::number(input.id))
                    ),
                    YADAW::Model::RegularAudioIOPositionModel::Role::Position
                ).value<QObject*>()
            );
        }
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
        auto channelIndex = *mixer.getChannelIndexOfId(
            Mixer::ChannelListType::AudioHardwareInputList, output.id
        );
        ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
            models.hardwareAudioInputPositionModel.data(
                models.hardwareAudioInputPositionModel.index(channelIndex),
                YADAW::Model::HardwareAudioIOPositionModel::Role::Position
            ).value<QObject*>()
        );
    }
    else if(output.type == Mixer::Position::Type::FXAndGroupChannelInput)
    {
        auto channelIndex = *mixer.getChannelIndexOfId(
            Mixer::ChannelListType::RegularList, output.id
        );
        if(auto channelType = mixer.channelInfoAt(
                Mixer::ChannelListType::RegularList, channelIndex
            )->get().channelType;
            channelType == Mixer::ChannelType::AudioFX)
        {
            ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
                models.audioFXIOPositionModel.data(
                    models.audioFXIOPositionModel.index(
                        models.audioFXIOPositionModel.findIndexByID(QString::number(output.id))
                    ),
                    YADAW::Model::RegularAudioIOPositionModel::Role::Position
                ).value<QObject*>()
            );
        }
        else if(channelType == Mixer::ChannelType::AudioBus)
        {
            ret = static_cast<YADAW::Entity::IAudioIOPosition*>(
                models.audioGroupIOPositionModel.data(
                    models.audioGroupIOPositionModel.index(
                        models.audioGroupIOPositionModel.findIndexByID(QString::number(output.id))
                    ),
                    YADAW::Model::RegularAudioIOPositionModel::Role::Position
                ).value<QObject*>()
            );
        }
    }
    else if(output.type == Mixer::Position::Type::Send)
    {
        auto sendPosition = *mixer.getSendPosition(output.id);
        auto& model = models.mixerChannels[sendPosition.channelListType];
        auto& sendModel = *static_cast<YADAW::Model::MixerChannelSendListModel*>(
            model.data(
                model.index(sendPosition.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::Sends
            ).value<QObject*>()
        );
        ret = &static_cast<YADAW::Entity::SendPosition&>(
            sendModel.positionAt(sendPosition.sendIndex)->get()
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

void mixerSendAdded(const YADAW::Audio::Mixer::Mixer& sender,
    const YADAW::Audio::Mixer::Mixer::SendPosition& sendPosition)
{
    using YADAW::Audio::Mixer::Mixer;
    assert(
        sender.sendCount(
            sendPosition.channelListType,
            sendPosition.channelIndex
        ).value_or(0) == sendPosition.sendIndex + 1
        &&
        "Is inserting sends not implemented?"
    );
    auto& models = appMixerChannelListModels();
    auto& model = models.mixerChannels[sendPosition.channelListType];
    auto sendModel = static_cast<YADAW::Model::MixerChannelSendListModel*>(
        model.data(
            model.index(sendPosition.channelIndex),
            YADAW::Model::MixerChannelListModel::Role::Sends
        ).value<QObject*>()
    );
    sendModel->appended();
}

void mixerSendDestinationChanged(const YADAW::Audio::Mixer::Mixer& sender,
    const YADAW::Audio::Mixer::Mixer::SendPosition& sendPosition)
{
    using YADAW::Audio::Mixer::Mixer;
    assert(
        sender.sendCount(
            sendPosition.channelListType,
            sendPosition.channelIndex
        ).value_or(0) == sendPosition.sendIndex + 1
        &&
        "Is inserting sends not implemented?"
    );
    auto& models = appMixerChannelListModels();
    auto& model = models.mixerChannels[sendPosition.channelListType];
    auto sendModel = static_cast<YADAW::Model::MixerChannelSendListModel*>(
        model.data(
            model.index(sendPosition.channelIndex),
            YADAW::Model::MixerChannelListModel::Role::Sends
        ).value<QObject*>()
    );
    sendModel->destinationChanged(sendPosition.sendIndex);
}

void mixerSendRemoved(const YADAW::Audio::Mixer::Mixer& sender,
    YADAW::Audio::Mixer::Mixer::SendRemovedCallbackArgs args)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& models = appMixerChannelListModels();
    auto& model = models.mixerChannels[args.sendPosition.channelListType];
    auto sendModel = static_cast<YADAW::Model::MixerChannelSendListModel*>(
        model.data(
            model.index(args.sendPosition.channelIndex),
            YADAW::Model::MixerChannelListModel::Role::Sends
        ).value<QObject*>()
    );
    sendModel->removed(args.sendPosition.sendIndex, args.sendPosition.sendIndex + args.removeCount - 1);
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
    hardwareAudioInputPositionModel(mixerChannels[0]),
    hardwareAudioOutputPositionModel(mixerChannels[2]),
    audioFXIOPositionModel(mixerChannels[1], YADAW::Model::MixerChannelListModel::ChannelTypes::ChannelTypeAudioFX),
    audioGroupIOPositionModel(mixerChannels[1], YADAW::Model::MixerChannelListModel::ChannelTypes::ChannelTypeBus),
    audioIOPositionModel(mixerChannels[1], YADAW::Model::MixerChannelListModel::ChannelTypes::ChannelTypeAudio),
    audioInputPositionModel(mixerChannels[0], mixerChannels[1], mixerChannels[2], true),
    audioOutputPositionModel(mixerChannels[0], mixerChannels[1], mixerChannels[2], false)
{
    mixer.setMainInputChangedCallback(&mixerMainInputChanged);
    mixer.setMainOutputChangedCallback(&mixerMainOutputChanged);
    mixer.setSendAddedCallback(&mixerSendAdded);
    mixer.setSendDestinationChangedCallback(&mixerSendDestinationChanged);
    mixer.setSendRemovedCallback(&mixerSendRemoved);
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
