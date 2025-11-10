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

void sendInserted(const YADAW::Model::MixerChannelSendListModel& sender, int first, int last)
{
    using YADAW::Audio::Mixer::Mixer;
    YADAW::Model::MixerChannelListModel* listModels[3] = {
        &(appAudioInputMixerChannels()),
        &(appMixerChannels()),
        &(appAudioOutputMixerChannels())
    };
    auto& mixer = listModels[0]->mixer();
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
    YADAW::Model::MixerChannelListModel* listModels[3] = {
        &(appAudioInputMixerChannels()),
        &(appMixerChannels()),
        &(appAudioOutputMixerChannels())
    };
    auto& mixer = listModels[0]->mixer();
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
    YADAW::Model::MixerChannelListModel* listModels[3] = {
        &(appAudioInputMixerChannels()),
        &(appMixerChannels()),
        &(appAudioOutputMixerChannels())
    };
    auto& mixer = listModels[0]->mixer();
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

void auxOutputAboutToBeRemoved(const YADAW::Model::AuxOutputDestinationListModel& sender, int first, int last)
{
    using YADAW::Audio::Mixer::Mixer;
    YADAW::Model::MixerChannelListModel* listModels[3] = {
        &(appAudioInputMixerChannels()),
        &(appMixerChannels()),
        &(appAudioOutputMixerChannels())
    };
    auto& mixer = listModels[0]->mixer();
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

void auxOutputAboutToBeChanged(const YADAW::Model::AuxOutputDestinationListModel& sender, int first, int last)
{
    auxOutputAboutToBeRemoved(sender, first, last);
}
}
