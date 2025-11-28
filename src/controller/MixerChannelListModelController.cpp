#include "MixerChannelListModelController.hpp"

#include "controller/AudioEngineController.hpp"
#include "model/MixerChannelInsertListModel.hpp"

namespace YADAW::Controller
{
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
    YADAW::Entity::IAudioIOPosition* outputAsPosition = nullptr;
    if(output.type == Mixer::Position::Type::AudioHardwareIOChannel)
    {
        auto channelIndex = *sender.getChannelIndexOfId(
            Mixer::ChannelListType::AudioHardwareInputList, output.id
        );
        outputAsPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
            models.hardwareAudioInputPositionModel.data(
                models.hardwareAudioInputPositionModel.index(channelIndex),
                YADAW::Model::HardwareAudioIOPositionModel::Role::Position
            ).value<QObject*>()
        );
    }
    else if(output.type == Mixer::Position::Type::SendAndFXChannel)
    {
        auto channelIndex = *sender.getChannelIndexOfId(
            Mixer::ChannelListType::RegularList, output.id
        );
        if(auto channelType = sender.channelInfoAt(
                Mixer::ChannelListType::RegularList, channelIndex
            )->get().channelType;
            channelType == Mixer::ChannelType::AudioFX)
        {
            outputAsPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
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
            outputAsPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
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
        auto sendPosition = *sender.getSendPosition(output.id);
        auto& model = models.mixerChannels[sendPosition.channelListType];
        auto& sendModel = *static_cast<YADAW::Model::MixerChannelSendListModel*>(
            model.data(
                model.index(sendPosition.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::Sends
            ).value<QObject*>()
        );
        outputAsPosition = &static_cast<YADAW::Entity::SendPosition&>(
            sendModel.positionAt(sendPosition.sendIndex)->get()
        );
    }
    else if(output.type == Mixer::Position::Type::PluginAuxIO)
    {
        outputAsPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
            models.audioOutputPositionModel.data(
                models.audioOutputPositionModel.findIndexByID(QString::number(output.id)),
                YADAW::Model::MixerAudioIOPositionItemModel::Role::Position
            ).value<QObject*>()
        );
    }
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
    auto& models = appMixerChannelListModels();
    auto& model = models.mixerChannels[args.auxOutput.channelListType];
    YADAW::Model::AuxOutputDestinationModel* auxOutputDestinationList = nullptr;
    if(args.auxOutput.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Instrument)
    {
        auxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            model.data(
                model.index(args.auxOutput.channelIndex),
                YADAW::Model::MixerChannelListModel::Role::InstrumentAudioAuxOutputDestination
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
        auxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
            insertListModel->data(
                insertListModel->index(args.auxOutput.insertIndex),
                YADAW::Model::MixerChannelInsertListModel::Role::AudioAuxOutputDestination
            ).value<QObject*>()
        );
    }
    assert(auxOutputDestinationList);
    auto& dest = destinations.back();
    YADAW::Entity::IAudioIOPosition* destAsPosition = nullptr;
    if(dest.type == Mixer::Position::Type::AudioHardwareIOChannel)
    {
        auto channelIndex = *sender.getChannelIndexOfId(
            Mixer::ChannelListType::AudioHardwareOutputList, dest.id
        );
        destAsPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
            models.hardwareAudioOutputPositionModel.data(
                models.hardwareAudioOutputPositionModel.index(channelIndex),
                YADAW::Model::HardwareAudioIOPositionModel::Role::Position
            ).value<QObject*>()
        );
    }
    else if(dest.type == Mixer::Position::Type::SendAndFXChannel)
    {
        auto channelIndex = *sender.getChannelIndexOfId(
            Mixer::ChannelListType::RegularList, dest.id
        );
        if(auto channelType = sender.channelInfoAt(
                Mixer::ChannelListType::RegularList, channelIndex
            )->get().channelType;
            channelType == Mixer::ChannelType::AudioFX)
        {
            destAsPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
                models.audioFXIOPositionModel.data(
                    models.audioFXIOPositionModel.index(
                        models.audioFXIOPositionModel.findIndexByID(QString::number(dest.id))
                    ),
                    YADAW::Model::RegularAudioIOPositionModel::Role::Position
                ).value<QObject*>()
            );
        }
        else if(channelType == Mixer::ChannelType::AudioBus)
        {
            destAsPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
                models.audioGroupIOPositionModel.data(
                    models.audioGroupIOPositionModel.index(
                        models.audioGroupIOPositionModel.findIndexByID(QString::number(dest.id))
                    ),
                    YADAW::Model::RegularAudioIOPositionModel::Role::Position
                ).value<QObject*>()
            );
        }
    }
    else if(dest.type == Mixer::Position::Type::AudioChannel)
    {
        auto channelIndex = *sender.getChannelIndexOfId(
            Mixer::ChannelListType::RegularList, dest.id
        );
        if(auto channelType = sender.channelInfoAt(
                Mixer::ChannelListType::RegularList, channelIndex
            )->get().channelType;
            channelType == Mixer::ChannelType::Audio)
        {
            destAsPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
                models.audioFXIOPositionModel.data(
                    models.audioFXIOPositionModel.index(
                        models.audioFXIOPositionModel.findIndexByID(QString::number(dest.id))
                    ),
                    YADAW::Model::RegularAudioIOPositionModel::Role::Position
                ).value<QObject*>()
            );
        }
    }
    else if(dest.type == Mixer::Position::Type::PluginAuxIO)
    {
        destAsPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
            models.audioInputPositionModel.data(
                models.audioInputPositionModel.findIndexByID(QString::number(dest.id)),
                YADAW::Model::MixerAudioIOPositionItemModel::Role::Position
            ).value<QObject*>()
        );
    }
    assert(destAsPosition);
    auxOutputDestinationList->added(destinations.size() - 1, destAsPosition);
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
    mixer.setSendAddedCallback(&mixerSendAdded);
    mixer.setSendDestinationChangedCallback(&mixerSendDestinationChanged);
    mixer.setSendRemovedCallback(&mixerSendRemoved);
    mixer.setAuxInputChangedCallback(&mixerAuxInputChanged);
    mixer.setAuxOutputAddedCallback(&mixerAuxOutputAdded);
}

MixerChannelListModels& appMixerChannelListModels()
{
    auto& mixer = YADAW::Controller::AudioEngine::appAudioEngine().mixer();
    static MixerChannelListModels ret(mixer);
    return ret;
}
}
