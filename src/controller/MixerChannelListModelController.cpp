#include "MixerChannelListModelController.hpp"

#include "controller/AudioEngineController.hpp"
#include "model/MixerChannelInsertListModel.hpp"

namespace YADAW::Controller
{
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
    audioInputPositionModel(mixerChannels[0], mixerChannels[1], mixerChannels[2], true),
    audioOutputPositionModel(mixerChannels[0], mixerChannels[1], mixerChannels[2], false)
{
    for(const auto& model: mixerChannels)
    {
        QObject::connect(
            &model, &YADAW::Model::MixerChannelListModel::rowsInserted,
            [&sender = model](const QModelIndex& parent, int first, int last)
            {
                FOR_RANGE(i, first, last + 1)
                {
                    // connect to sends
                    auto sendListModel = static_cast<YADAW::Model::MixerChannelSendListModel*>(
                        sender.data(
                            sender.index(i), YADAW::Model::MixerChannelListModel::Role::Sends
                        ).value<QObject*>()
                    );
                    QObject::connect(
                        sendListModel, &YADAW::Model::MixerChannelSendListModel::rowsInserted,
                        [&sender = *sendListModel](const QModelIndex& parent, int first, int last)
                        {
                            YADAW::Controller::sendInserted(sender, first, last);
                        }
                    );
                    QObject::connect(
                        sendListModel, &YADAW::Model::MixerChannelSendListModel::destinationAboutToBeChanged,
                        [&sender = *sendListModel](int first, int last)
                        {
                            YADAW::Controller::sendAboutToBeChanged(sender, first, last);
                        }
                    );
                    QObject::connect(
                        sendListModel, &YADAW::Model::MixerChannelSendListModel::rowsAboutToBeRemoved,
                        [&sender = *sendListModel](const QModelIndex& parent, int first, int last)
                        {
                            YADAW::Controller::sendAboutToBeRemoved(sender, first, last);
                        }
                    );
                    // connect to aux outputs
                    auto insertListModel = static_cast<YADAW::Model::MixerChannelInsertListModel*>(
                        sender.data(
                            sender.index(i),
                            YADAW::Model::MixerChannelListModel::Role::Inserts
                        ).value<QObject*>()
                    );
                    QObject::connect(
                        insertListModel, &YADAW::Model::MixerChannelInsertListModel::rowsInserted,
                        [&sender = *insertListModel](const QModelIndex& parent, int first, int last)
                        {
                            FOR_RANGE(i, first, last + 1)
                            {
                                auto insertAuxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationListModel*>(
                                    sender.data(
                                        sender.index(i),
                                        YADAW::Model::MixerChannelInsertListModel::Role::AudioAuxOutputDestination
                                    ).value<QObject*>()
                                );
                                FOR_RANGE0(j, insertAuxOutputDestinationList->rowCount({}))
                                {
                                    auto auxOutputDestinations = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
                                        insertAuxOutputDestinationList->data(
                                            insertAuxOutputDestinationList->index(j),
                                            YADAW::Model::AuxOutputDestinationModel::Role::Destination
                                        ).value<QObject*>()
                                    );
                                    QObject::connect(
                                        auxOutputDestinations, &YADAW::Model::AuxOutputDestinationModel::rowsInserted,
                                        [&sender = *auxOutputDestinations](const QModelIndex& parent, int first, int last)
                                        {
                                            auxOutputInserted(sender, first, last);
                                        }
                                    );
                                    QObject::connect(
                                        auxOutputDestinations, &YADAW::Model::AuxOutputDestinationModel::destinationAboutToBeChanged,
                                        [&sender = *auxOutputDestinations](int first, int last)
                                        {
                                            auxOutputAboutToBeChanged(sender, first, last);
                                        }
                                    );
                                    QObject::connect(
                                        auxOutputDestinations, &YADAW::Model::AuxOutputDestinationModel::rowsAboutToBeRemoved,
                                        [&sender = *auxOutputDestinations](const QModelIndex& parent, int first, int last)
                                        {
                                            auxOutputAboutToBeRemoved(sender, first, last);
                                        }
                                    );
                                }
                            }
                        }
                    );
                }
            }
        );
    }
    QObject::connect(
        &mixerChannels[1], &YADAW::Model::MixerChannelListModel::dataChanged,
        [&sender = mixerChannels[1]](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
        {
            if(roles.empty() || roles.contains(YADAW::Model::MixerChannelListModel::InstrumentAudioAuxOutputDestination))
            {
                FOR_RANGE(i, topLeft.row(), bottomRight.row() + 1)
                {
                    auto auxOutputDestinationList = static_cast<YADAW::Model::AuxOutputDestinationListModel*>(
                        sender.data(
                            sender.index(i),
                            YADAW::Model::MixerChannelListModel::InstrumentAudioAuxOutputDestination
                        ).value<QObject*>()
                    );
                    FOR_RANGE0(j, auxOutputDestinationList? auxOutputDestinationList->rowCount({}): 0)
                    {
                        auto auxOutputDestinations = static_cast<YADAW::Model::AuxOutputDestinationModel*>(
                            auxOutputDestinationList->data(
                                auxOutputDestinationList->index(j),
                                YADAW::Model::AuxOutputDestinationModel::Role::Destination
                            ).value<QObject*>()
                        );
                        QObject::connect(
                            auxOutputDestinations, &YADAW::Model::AuxOutputDestinationModel::rowsInserted,
                            [&sender = *auxOutputDestinations](const QModelIndex& parent, int first, int last)
                            {
                                auxOutputInserted(sender, first, last);
                            }
                        );
                        QObject::connect(
                            auxOutputDestinations, &YADAW::Model::AuxOutputDestinationModel::destinationAboutToBeChanged,
                            [&sender = *auxOutputDestinations](int first, int last)
                            {
                                auxOutputAboutToBeChanged(sender, first, last);
                            }
                        );
                        QObject::connect(
                            auxOutputDestinations, &YADAW::Model::AuxOutputDestinationModel::rowsAboutToBeRemoved,
                            [&sender = *auxOutputDestinations](const QModelIndex& parent, int first, int last)
                            {
                                auxOutputAboutToBeRemoved(sender, first, last);
                            }
                        );
                    }
                }
            }
        }
    );
}

MixerChannelListModels& appMixerChannelListModels()
{
    auto& mixer = YADAW::Controller::AudioEngine::appAudioEngine().mixer();
    static MixerChannelListModels ret(mixer);
    return ret;
}

YADAW::Model::MixerChannelListModel& appAudioInputMixerChannels()
{
    return appMixerChannelListModels().mixerChannels[0];
}

YADAW::Model::MixerChannelListModel& appMixerChannels()
{
    return appMixerChannelListModels().mixerChannels[1];
}

YADAW::Model::MixerChannelListModel& appAudioOutputMixerChannels()
{
    return appMixerChannelListModels().mixerChannels[2];
}

YADAW::Model::MixerAudioIOPositionItemModel& appMixerAudioInputPositionModel()
{
    return appMixerChannelListModels().audioInputPositionModel;
}

YADAW::Model::MixerAudioIOPositionItemModel& appMixerAudioOutputPositionModel()
{
    return appMixerChannelListModels().audioOutputPositionModel;
}

void sendInserted(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& mixer = appMixerChannelListModels().mixerChannels[0].mixer();
    FOR_RANGE(i, first, last + 1)
    {
        auto pPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
            sender.data(
                sender.index(i),
                YADAW::Model::MixerChannelSendListModel::Role::Destination
            ).value<QObject*>()
        );
        if(pPosition->getType() == YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO)
        {
            auto position = static_cast<Mixer::Position>(
                *static_cast<YADAW::Entity::PluginAuxAudioIOPosition*>(pPosition)
            );
            auto auxInputPosition = *mixer.getAuxInputPosition(position.id);
            auto optContext = auxInputPosition.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Instrument?
                mixer.getInstrumentContext(auxInputPosition.channelIndex):
                (
                    auxInputPosition.isPreFaderInsert?
                        mixer.preFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex):
                        mixer.postFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex)
                )->get().insertContextAt(auxInputPosition.insertIndex);
            auto* pluginContext = static_cast<YADAW::Controller::PluginContext*>(optContext->get().get());
            auto* pluginContextUserData = static_cast<YADAW::Model::PluginContextUserData*>(pluginContext->userData.get());
            auto inputSourceIndex = auxInputPosition.channelGroupIndex;
            if(auxInputPosition.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Inserts)
            {
                auto inputIndex = *(
                    auxInputPosition.isPreFaderInsert?
                        mixer.preFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex):
                        mixer.postFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex)
                )->get().insertInputChannelGroupIndexAt(auxInputPosition.insertIndex);
                if(inputSourceIndex > inputIndex)
                {
                    --inputSourceIndex;
                }
            }
            auto& sendPosition = sender.positionAt(i)->get();
            pluginContextUserData->audioAuxInputSources->inputChanged(
                inputSourceIndex, &sendPosition
            );
        }
    }
}

void sendAboutToBeRemoved(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& mixer = appMixerChannelListModels().mixerChannels[0].mixer();
    FOR_RANGE(i, first, last + 1)
    {
        auto pPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
            sender.data(
                sender.index(i),
                YADAW::Model::MixerChannelSendListModel::Role::Destination
            ).value<QObject*>()
        );
        if(pPosition->getType() == YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO)
        {
            auto position = static_cast<Mixer::Position>(
                *static_cast<YADAW::Entity::PluginAuxAudioIOPosition*>(pPosition)
            );
            auto auxInputPosition = *mixer.getAuxInputPosition(position.id);
            auto optContext = auxInputPosition.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Instrument?
                mixer.getInstrumentContext(auxInputPosition.channelIndex):
                (
                    auxInputPosition.isPreFaderInsert?
                        mixer.preFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex):
                        mixer.postFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex)
                )->get().insertContextAt(auxInputPosition.insertIndex);
            auto* pluginContext = static_cast<YADAW::Controller::PluginContext*>(optContext->get().get());
            auto* pluginContextUserData = static_cast<YADAW::Model::PluginContextUserData*>(pluginContext->userData.get());
            auto inputSourceIndex = auxInputPosition.channelGroupIndex;
            if(auxInputPosition.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Inserts)
            {
                auto inputIndex = *(
                    auxInputPosition.isPreFaderInsert?
                        mixer.preFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex):
                        mixer.postFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex)
                )->get().insertInputChannelGroupIndexAt(auxInputPosition.insertIndex);
                if(inputSourceIndex > inputIndex)
                {
                    --inputSourceIndex;
                }
            }
            pluginContextUserData->audioAuxInputSources->inputChanged(
                inputSourceIndex, nullptr
            );
        }
    }
}

void sendAboutToBeChanged(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last)
{
    sendAboutToBeRemoved(sender, first, last);
}

void auxOutputInserted(const YADAW::Model::AuxOutputDestinationModel& sender, int first, int last)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& mixer = appMixerChannelListModels().mixerChannels[0].mixer();
    YADAW::Entity::IAudioIOPosition* auxOutputPosition = nullptr;
    FOR_RANGE(i, first, last + 1)
    {
        auto pDest = static_cast<YADAW::Entity::IAudioIOPosition*>(
            sender.data(
                sender.index(i), YADAW::Model::AuxOutputDestinationModel::Role::Destination
            ).value<QObject*>()
        );
        if(pDest->getType() == YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO)
        {
            auto dest = static_cast<Mixer::Position>(
                *static_cast<YADAW::Entity::PluginAuxAudioIOPosition*>(pDest)
            );
            auto auxInputPosition = *mixer.getAuxInputPosition(dest.id);
            auto optContext = auxInputPosition.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Instrument?
                mixer.getInstrumentContext(auxInputPosition.channelIndex):
                (
                    auxInputPosition.isPreFaderInsert?
                        mixer.preFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex):
                        mixer.postFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex)
                )->get().insertContextAt(auxInputPosition.insertIndex);
            auto* pluginContext = static_cast<YADAW::Controller::PluginContext*>(optContext->get().get());
            auto* pluginContextUserData = static_cast<YADAW::Model::PluginContextUserData*>(pluginContext->userData.get());
            auto inputSourceIndex = auxInputPosition.channelGroupIndex;
            if(auxInputPosition.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Inserts)
            {
                auto inputIndex = *(
                    auxInputPosition.isPreFaderInsert?
                        mixer.preFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex):
                        mixer.postFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex)
                )->get().insertInputChannelGroupIndexAt(auxInputPosition.insertIndex);
                if(inputSourceIndex > inputIndex)
                {
                    --inputSourceIndex;
                }
            }
            if(!auxOutputPosition)
            {
                auto auxOutput = sender.getModel().position(sender.getChannelGroupIndex());
                auto& auxOutputModel = YADAW::Controller::appMixerAudioOutputPositionModel();
                auto modelIndex = auxOutputModel.findIndexByID(QString::number(dest.id));
                auxOutputPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
                    auxOutputModel.data(modelIndex, YADAW::Model::MixerAudioIOPositionItemModel::Role::Position).value<QObject*>()
                );
            }
            pluginContextUserData->audioAuxInputSources->inputChanged(
                inputSourceIndex, auxOutputPosition
            );
        }
    }
}

void auxOutputAboutToBeRemoved(const YADAW::Model::AuxOutputDestinationModel& sender, int first, int last)
{
    using YADAW::Audio::Mixer::Mixer;
    auto& mixer = appMixerChannelListModels().mixerChannels[0].mixer();
    YADAW::Entity::IAudioIOPosition* auxOutputPosition = nullptr;
    FOR_RANGE(i, first, last + 1)
    {
        auto pPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
            sender.data(
                sender.index(i), YADAW::Model::AuxOutputDestinationModel::Role::Destination
            ).value<QObject*>()
        );
        if(pPosition->getType() == YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO)
        {
            auto position = static_cast<Mixer::Position>(
                *static_cast<YADAW::Entity::PluginAuxAudioIOPosition*>(pPosition)
            );
            auto auxInputPosition = *mixer.getAuxInputPosition(position.id);
            auto optContext = auxInputPosition.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Instrument?
                mixer.getInstrumentContext(auxInputPosition.channelIndex):
                (
                    auxInputPosition.isPreFaderInsert?
                        mixer.preFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex):
                        mixer.postFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex)
                )->get().insertContextAt(auxInputPosition.insertIndex);
            auto* pluginContext = static_cast<YADAW::Controller::PluginContext*>(optContext->get().get());
            auto* pluginContextUserData = static_cast<YADAW::Model::PluginContextUserData*>(pluginContext->userData.get());
            auto inputSourceIndex = auxInputPosition.channelGroupIndex;
            if(auxInputPosition.inChannelPosition == Mixer::PluginAuxIOPosition::InChannelPosition::Inserts)
            {
                auto inputIndex = *(
                    auxInputPosition.isPreFaderInsert?
                        mixer.preFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex):
                        mixer.postFaderInsertsAt(auxInputPosition.channelListType, auxInputPosition.channelIndex)
                )->get().insertInputChannelGroupIndexAt(auxInputPosition.insertIndex);
                if(inputSourceIndex > inputIndex)
                {
                    --inputSourceIndex;
                }
            }
            pluginContextUserData->audioAuxInputSources->inputChanged(
                inputSourceIndex, nullptr
            );
        }
    }
}

void auxOutputAboutToBeChanged(const YADAW::Model::AuxOutputDestinationModel& sender, int first, int last)
{
    auxOutputAboutToBeRemoved(sender, first, last);
}
}
