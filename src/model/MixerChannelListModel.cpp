#include "MixerChannelListModel.hpp"

#include "RegularAudioIOPositionModel.hpp"
#include "audio/plugin/CLAPPlugin.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "audio/util/InputSwitcher.hpp"
#include "controller/AudioEngineController.hpp"
#include "controller/AudioGraphBackendController.hpp"
#include "controller/ALSABackendController.hpp"
#include "controller/CoreAudioBackendController.hpp"
#include "controller/PluginController.hpp"
#include "controller/PluginIOConfigUpdatedCallback.hpp"
#include "controller/PluginLatencyUpdatedCallback.hpp"
#include "controller/PluginWindowController.hpp"
#include "dao/PluginTable.hpp"
#include "entity/ChannelConfigHelper.hpp"
#include "entity/HardwareAudioIOPosition.hpp"
#include "entity/PluginAuxAudioIOPosition.hpp"
#include "entity/RegularAudioIOPosition.hpp"
#include "event/EventBase.hpp"
#include "model/MixerChannelInsertListModel.hpp"
#include "model/MixerChannelSendListModel.hpp"
#include "util/Base.hpp"
#include "util/IntegerRange.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
YADAW::Audio::Mixer::Mixer::ChannelType channelTypeFromModelTypes(
    IMixerChannelListModel::ChannelTypes type
)
{
    return static_cast<YADAW::Audio::Mixer::Mixer::ChannelType>(type);
}

using GetCount = decltype(&YADAW::Audio::Mixer::Mixer::channelCount);

GetCount getCount[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputChannelCount,
    &YADAW::Audio::Mixer::Mixer::channelCount,
    &YADAW::Audio::Mixer::Mixer::audioOutputChannelCount,
};

using GetInserts =
    OptionalRef<YADAW::Audio::Mixer::Inserts>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t);

GetInserts getPreFaderInserts[3] = {
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPreFaderInsertsAt),
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::channelPreFaderInsertsAt),
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPreFaderInsertsAt)
};

GetInserts getPostFaderInserts[3] = {
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPostFaderInsertsAt),
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::channelPostFaderInsertsAt),
    static_cast<GetInserts>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPostFaderInsertsAt)
};

using GetConstInserts =
    OptionalRef<const YADAW::Audio::Mixer::Inserts>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t) const;

GetConstInserts getConstPreFaderInserts[3] = {
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPreFaderInsertsAt),
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::channelPreFaderInsertsAt),
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPreFaderInsertsAt)
};

GetConstInserts getConstPostFaderInserts[3] = {
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPostFaderInsertsAt),
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::channelPostFaderInsertsAt),
    static_cast<GetConstInserts>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPostFaderInsertsAt)
};

using GetFader =
    OptionalRef<YADAW::Audio::Mixer::VolumeFader>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t);

GetFader getFader[3] = {
    static_cast<GetFader>(&YADAW::Audio::Mixer::Mixer::audioInputVolumeFaderAt),
    static_cast<GetFader>(&YADAW::Audio::Mixer::Mixer::volumeFaderAt),
    static_cast<GetFader>(&YADAW::Audio::Mixer::Mixer::audioOutputVolumeFaderAt)
};

using GetConstFader =
    OptionalRef<const YADAW::Audio::Mixer::VolumeFader>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t) const;

GetConstFader getConstFader[3] = {
    static_cast<GetConstFader>(&YADAW::Audio::Mixer::Mixer::audioInputVolumeFaderAt),
    static_cast<GetConstFader>(&YADAW::Audio::Mixer::Mixer::volumeFaderAt),
    static_cast<GetConstFader>(&YADAW::Audio::Mixer::Mixer::audioOutputVolumeFaderAt)
};

using GetChannelInfo =
    OptionalRef<YADAW::Audio::Mixer::Mixer::ChannelInfo>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t);

GetChannelInfo getChannelInfo[3] = {
    static_cast<GetChannelInfo>(&YADAW::Audio::Mixer::Mixer::audioInputChannelInfoAt),
    static_cast<GetChannelInfo>(&YADAW::Audio::Mixer::Mixer::channelInfoAt),
    static_cast<GetChannelInfo>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelInfoAt)
};

using GetConstChannelInfo =
    OptionalRef<const YADAW::Audio::Mixer::Mixer::ChannelInfo>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t) const;

GetConstChannelInfo getConstChannelInfo[3] = {
    static_cast<GetConstChannelInfo>(&YADAW::Audio::Mixer::Mixer::audioInputChannelInfoAt),
    static_cast<GetConstChannelInfo>(&YADAW::Audio::Mixer::Mixer::channelInfoAt),
    static_cast<GetConstChannelInfo>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelInfoAt)
};

using GetConstID =
    std::optional<YADAW::Audio::Mixer::IDGen::ID>(YADAW::Audio::Mixer::Mixer::*)(
        std::uint32_t) const;

GetConstID getConstID[3] = {
    &Audio::Mixer::Mixer::audioInputChannelIDAt,
    &Audio::Mixer::Mixer::channelIDAt,
    &Audio::Mixer::Mixer::audioOutputChannelIDAt
};

using RemoveChannels =
    decltype(&YADAW::Audio::Mixer::Mixer::removeChannel);

RemoveChannels removeChannels[3] = {
    &YADAW::Audio::Mixer::Mixer::removeAudioInputChannel,
    &YADAW::Audio::Mixer::Mixer::removeChannel,
    &YADAW::Audio::Mixer::Mixer::removeAudioOutputChannel
};

using ClearChannels =
    decltype(&YADAW::Audio::Mixer::Mixer::clearChannels);

ClearChannels clearChannels[3] = {
    &YADAW::Audio::Mixer::Mixer::clearAudioInputChannels,
    &YADAW::Audio::Mixer::Mixer::clearChannels,
    &YADAW::Audio::Mixer::Mixer::clearAudioOutputChannels,
};

using GetIndexOfId =
    decltype(&YADAW::Audio::Mixer::Mixer::getIndexOfId);

GetIndexOfId getIndexOfIdFunc[3] = {
    &YADAW::Audio::Mixer::Mixer::getInputIndexOfId,
    &YADAW::Audio::Mixer::Mixer::getIndexOfId,
    &YADAW::Audio::Mixer::Mixer::getOutputIndexOfId,
};

using GetChannelGroupType =
    decltype(&YADAW::Audio::Mixer::Mixer::audioInputChannelGroupTypeAt);

GetChannelGroupType getChannelGroupType[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputChannelGroupTypeAt,
    &YADAW::Audio::Mixer::Mixer::channelGroupTypeAt,
    &YADAW::Audio::Mixer::Mixer::audioOutputChannelGroupTypeAt
};

using GetChannelGroupTypeAndCount =
    decltype(&YADAW::Audio::Mixer::Mixer::audioInputChannelGroupTypeAndChannelCountAt);

GetChannelGroupTypeAndCount getChannelGroupTypeAndCount[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputChannelGroupTypeAndChannelCountAt,
    &YADAW::Audio::Mixer::Mixer::channelGroupTypeAndChannelCountAt,
    &YADAW::Audio::Mixer::Mixer::audioOutputChannelGroupTypeAndChannelCountAt
};

using GetConstMuteAt =
    OptionalRef<const YADAW::Audio::Util::Mute>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t index) const;

GetConstMuteAt getConstMuteAt[3] = {
    static_cast<GetConstMuteAt>(&YADAW::Audio::Mixer::Mixer::audioInputMuteAt),
    static_cast<GetConstMuteAt>(&YADAW::Audio::Mixer::Mixer::muteAt),
    static_cast<GetConstMuteAt>(&YADAW::Audio::Mixer::Mixer::audioOutputMuteAt)
};

using GetMuteAt =
    OptionalRef<YADAW::Audio::Util::Mute>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t index);

GetMuteAt getMuteAt[3] = {
    static_cast<GetMuteAt>(&YADAW::Audio::Mixer::Mixer::audioInputMuteAt),
    static_cast<GetMuteAt>(&YADAW::Audio::Mixer::Mixer::muteAt),
    static_cast<GetMuteAt>(&YADAW::Audio::Mixer::Mixer::audioOutputMuteAt)
};

using HasMute =
    decltype(&YADAW::Audio::Mixer::Mixer::hasMuteInRegularChannels);

HasMute hasMuteFunc[3] = {
    &YADAW::Audio::Mixer::Mixer::hasMuteInAudioInputChannels,
    &YADAW::Audio::Mixer::Mixer::hasMuteInRegularChannels,
    &YADAW::Audio::Mixer::Mixer::hasMuteInAudioOutputChannels
};

using UnmuteAll =
    decltype(&YADAW::Audio::Mixer::Mixer::unmuteRegularChannels);

UnmuteAll unmuteAllFunc[3] = {
    &YADAW::Audio::Mixer::Mixer::unmuteAudioInputChannels,
    &YADAW::Audio::Mixer::Mixer::unmuteRegularChannels,
    &YADAW::Audio::Mixer::Mixer::unmuteAudioOutputChannels
};

using GetConstPolarityInverterFunc =
    OptionalRef<const YADAW::Audio::Mixer::PolarityInverter>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t index) const;

GetConstPolarityInverterFunc getConstPolarityInverters[3] = {
    static_cast<GetConstPolarityInverterFunc>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPolarityInverterAt),
    static_cast<GetConstPolarityInverterFunc>(&YADAW::Audio::Mixer::Mixer::channelPolarityInverterAt),
    static_cast<GetConstPolarityInverterFunc>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPolarityInverterAt)
};

using GetPolarityInverterFunc =
    OptionalRef<YADAW::Audio::Mixer::PolarityInverter>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t index);

GetPolarityInverterFunc getPolarityInverters[3] = {
    static_cast<GetPolarityInverterFunc>(&YADAW::Audio::Mixer::Mixer::audioInputChannelPolarityInverterAt),
    static_cast<GetPolarityInverterFunc>(&YADAW::Audio::Mixer::Mixer::channelPolarityInverterAt),
    static_cast<GetPolarityInverterFunc>(&YADAW::Audio::Mixer::Mixer::audioOutputChannelPolarityInverterAt)
};

IMixerChannelListModel::ChannelTypes getChannelType(YADAW::Audio::Mixer::Mixer::ChannelType type)
{
    return static_cast<IMixerChannelListModel::ChannelTypes>(YADAW::Util::underlyingValue(type));
}

MixerChannelListModel::MixerChannelListModel(
    YADAW::Audio::Mixer::Mixer& mixer, ListType listType, QObject* parent):
    IMixerChannelListModel(parent),
    mixer_(mixer),
    listType_(listType),
    fillPluginContextCallback_(&Impl::blankFillPluginContext)
{
    YADAW::Util::setCppOwnership(*this);
    auto count = itemCount();
    insertModels_.reserve(count);
    sendModels_.reserve(count);
    polarityInverterModels_.reserve(count);
    editingVolume_.resize(count, false);
    std::generate_n(
        std::back_inserter(insertModels_), count,
        [this, i = 0U]() mutable
        {
            auto index = i++;
            return std::make_unique<YADAW::Model::MixerChannelInsertListModel>(
                (mixer_.*getPreFaderInserts[YADAW::Util::underlyingValue(listType_)])(index)->get(),
                listType_, true, index, 0
            );
        }
    );
    std::generate_n(
        std::back_inserter(sendModels_), count,
        [this, i = 0U]() mutable
        {
            return std::make_unique<YADAW::Model::MixerChannelSendListModel>(
                mixer_, listType_, i++
            );
        }
    );
    std::generate_n(
        std::back_inserter(polarityInverterModels_), count,
        [this, i = 0U]() mutable
        {
            auto index = i++;
            return std::make_unique<PolarityInverterModel>(
                (mixer_.*getPolarityInverters[YADAW::Util::underlyingValue(listType_)])(index)->get()
            );
        }
    );
    if(listType_ == ListType::Regular)
    {
        std::fill_n(std::back_inserter(mainInput_), count, nullptr);
        std::fill_n(std::back_inserter(mainOutput_), count, nullptr);
    }
    std::function<YADAW::Audio::Mixer::Mixer::PreInsertChannelCallback> callback = [this](
        const YADAW::Audio::Mixer::Mixer&,
        YADAW::Audio::Mixer::Mixer::PreInsertChannelCallbackArgs args)
    {
        if(args.count > 0)
        {
            beginInsertRows(
                QModelIndex(),
                args.position,
                args.position + args.count - 1
            );
        }
    };
    if(listType_ == ListType::AudioHardwareInput)
    {
        mixer_.setPreInsertAudioInputChannelCallback(std::move(callback));
    }
    else if(listType == ListType::Regular)
    {
        mixer_.setPreInsertRegularChannelCallback(std::move(callback));
    }
    else if(listType_ == ListType::AudioHardwareOutput)
    {
        mixer_.setPreInsertAudioOutputChannelCallback(std::move(callback));
    }
}

MixerChannelListModel::~MixerChannelListModel()
{}

int MixerChannelListModel::itemCount() const
{
    return (mixer_.*getCount[YADAW::Util::underlyingValue(listType_)])();
}

int MixerChannelListModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant MixerChannelListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Id:
        {
            auto optionalId = (mixer_.*getConstID[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalId.has_value())
            {
                return QVariant::fromValue(QString::number(*optionalId));
            }
            return QVariant();
        }
        case Role::Name:
        {
            const auto& optionalInfo = (mixer_.*getConstChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                return QVariant::fromValue(optionalInfo->get().name);
            }
            return QVariant();
        }
        case Role::NameWithIndex:
        {
            const auto& optionalInfo = (mixer_.*getConstChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                return QVariant::fromValue(QString("%1: %2").arg(QString::number(row + 1), optionalInfo->get().name));
            }
            return QVariant();
        }
        case Role::Color:
        {
            const auto& optionalInfo = (mixer_.*getConstChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                return QVariant::fromValue(optionalInfo->get().color);
            }
            return QVariant();
        }
        case Role::ChannelType:
        {
            auto optionalInfo = (mixer_.*getConstChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                return QVariant::fromValue(getChannelType(optionalInfo->get().channelType));
            }
            return QVariant();
        }
        case Role::ChannelConfig:
        {
            auto optionalChannelGroupTypeAndCount = (mixer_.*getChannelGroupTypeAndCount[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalChannelGroupTypeAndCount.has_value())
            {
                auto ret = YADAW::Entity::configFromGroupType(
                    optionalChannelGroupTypeAndCount->first
                );
                return ret;
            }
            break;
        }
        case Role::ChannelCount:
        {
            auto optionalChannelGroupTypeAndCount = (mixer_.*getChannelGroupTypeAndCount[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalChannelGroupTypeAndCount.has_value())
            {
                return QVariant::fromValue<int>(optionalChannelGroupTypeAndCount->second);
            }
            break;
        }
        case Role::Input:
        {
            if(listType_ == ListType::Regular && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Audio)
            {
                return QVariant::fromValue<QObject*>(mainInput_[row]);
            }
            return QVariant();
        }
        case Role::InputType:
        {
            auto optionalInfo = (mixer_.*getConstChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                auto channelType = optionalInfo->get().channelType;
                if(channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
                {
                    return MediaTypes::MediaTypeMIDI;
                }
                return MediaTypes::MediaTypeAudio;
            }
            break;
        }
        case Role::Output:
        {
            if(listType_ == ListType::Regular)
            {
                return QVariant::fromValue<QObject*>(mainOutput_[row]);
            }
            return QVariant();
        }
        case Role::OutputType:
        {
            return MediaTypes::MediaTypeAudio;
        }
        case Role::InstrumentExist:
        {
            return QVariant::fromValue(
                listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument
                && mixer_.getInstrumentContext(row)
            );
        }
        case Role::InstrumentBypassed:
        {
            return listType_ == ListType::Regular
                && mixer_.isInstrumentBypassed(row);
        }
        case Role::InstrumentName:
        {
            if(listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
            {
                auto context = mixer_.getInstrumentContext(row);
                if(context.has_value())
                {
                    auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context->get().get());
                    auto& instrumentContext = *static_cast<PluginContextUserData*>(pluginContext.userData.get());
                    return QVariant::fromValue(instrumentContext.name);
                }
            }
            return {};
        }
        case Role::InstrumentAudioInputs:
        {
            if(listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
            {
                auto context = mixer_.getInstrumentContext(row);
                if(context.has_value())
                {
                    auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context->get().get());
                    auto& instrumentContext = *static_cast<PluginContextUserData*>(pluginContext.userData.get());
                    return QVariant::fromValue(&(instrumentContext.audioInputs));
                }
            }
            return {};
        }
        case Role::InstrumentAudioOutputs:
        {
            if(listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
            {
                auto context = mixer_.getInstrumentContext(row);
                if(context.has_value())
                {
                    auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context->get().get());
                    auto& instrumentContext = *static_cast<PluginContextUserData*>(pluginContext.userData.get());
                    return QVariant::fromValue(&(instrumentContext.audioOutputs));
                }
            }
            return {};
        }
        case Role::InstrumentHasUI:
        {
            if(listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
            {
                auto context = mixer_.getInstrumentContext(row);
                if(context.has_value())
                {
                    auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context->get().get());
                    auto& plugin = pluginContext.pluginInstance.plugin()->get();
                    return static_cast<bool>(plugin.gui());
                }
            }
            return false;
        }
        case Role::InstrumentWindowVisible:
        {
            if(listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
            {
                auto context = mixer_.getInstrumentContext(row);
                if(context.has_value())
                {
                    auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context->get().get());
                    return QVariant::fromValue<bool>(pluginContext.editor && pluginContext.editor->isVisible());
                }
            }
            return {};
        }
        case Role::InstrumentGenericEditorVisible:
        {
            if(listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
            {
                auto context = mixer_.getInstrumentContext(row);
                if(context.has_value())
                {
                    auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context->get().get());
                    return QVariant::fromValue<bool>(pluginContext.genericEditor && pluginContext.genericEditor->isVisible());
                }
            }
            return {};
        }
        case Role::InstrumentLatency:
        {
            if(listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
            {
                auto context = mixer_.getInstrumentContext(row);
                if(context.has_value())
                {
                    auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context->get().get());
                    return QVariant::fromValue<int>(pluginContext.pluginInstance.plugin()->get().latencyInSamples());
                }
            }
            return {};
        }
        case Role::Inserts:
        {
            return QVariant::fromValue<QObject*>(insertModels_[row].get());
        }
        case Role::Sends:
        {
            return QVariant::fromValue<QObject*>(sendModels_[row].get());
        }
        case Role::Mute:
        {
            const auto& mute = (mixer_.*getConstMuteAt[listType_])(row)->get();
            return QVariant::fromValue<bool>(
                mute.getMute()
            );
        }
        case Role::PolarityInverter:
        {
            return QVariant::fromValue<QObject*>(
                polarityInverterModels_[row].get()
            );
        }
        case Role::MonitorExist:
        {
            if(listType_ == ListType::Regular)
            {
                const auto& channelInfo = mixer_.channelInfoAt(row)->get();
                return QVariant::fromValue<bool>(
                    channelInfo.channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Audio
                );
            }
            return QVariant::fromValue<bool>(false);
        }
        case Role::Monitor:
        {
            if(data(index, Role::MonitorExist).value<bool>())
            {
                auto multiInput = static_cast<YADAW::Audio::Engine::MultiInputDeviceWithPDC*>(
                    mixer_.graph().graph().getNodeData(
                        mixer_.channelPreFaderInsertsAt(row)->get().inNode()->inNodes().front()
                    ).process.device()
                );
                auto inputSwitcher = static_cast<YADAW::Audio::Util::InputSwitcher*>(
                    multiInput->process().device()
                );
                return QVariant::fromValue<bool>(inputSwitcher->getInputIndex());
            }
            break;
        }
        case Role::Volume:
        {
            return QVariant::fromValue<double>(
                (mixer_.*getConstFader[listType_])(row)->get()
                .parameter(0)->value()
            );
        }
        case Role::EditingVolume:
        {
            return QVariant::fromValue<bool>(editingVolume_[row]);
        }
        }
    }
    return QVariant();
}

bool MixerChannelListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
        {
            auto optionalInfo = (mixer_.*getChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                optionalInfo->get().name = value.value<QString>();
                dataChanged(index, index, {Role::Name, Role::NameWithIndex});
                return true;
            }
            return false;
        }
        case Role::Color:
        {
            auto optionalInfo = (mixer_.*getChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                optionalInfo->get().color = value.value<QColor>();
                dataChanged(index, index, {Role::Color});
                return true;
            }
            return false;
        }
        case Role::Input:
        {
            auto ret = false;
            if(listType_ == ListType::Regular)
            {
                auto pPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(value.value<QObject*>());
                if(!pPosition)
                {
                    ret = mixer_.setMainInputAt(
                        row,
                        YADAW::Audio::Mixer::Mixer::Position {
                            YADAW::Audio::Mixer::Mixer::Position::Type::Invalid
                        }
                    );
                }
                else
                {
                    switch(pPosition->getType())
                    {
                    case YADAW::Entity::IAudioIOPosition::Type::AudioHardwareIOChannel:
                    {
                        const auto& hardwareAudioIOPosition = static_cast<const YADAW::Entity::HardwareAudioIOPosition&>(*pPosition);
                        ret = mixer_.setMainInputAt(
                            row,
                            static_cast<YADAW::Audio::Mixer::Mixer::Position>(hardwareAudioIOPosition)
                        );
                        break;
                    }
                    case YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel:
                    {
                        const auto& regularAudioIOPosition = static_cast<const YADAW::Entity::RegularAudioIOPosition&>(*pPosition);
                        ret = mixer_.setMainInputAt(
                            row,
                            static_cast<YADAW::Audio::Mixer::Mixer::Position>(regularAudioIOPosition)
                        );
                        break;
                    }
                    case YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO:
                    {
                        // not implemented
                        break;
                    }
                    }
                }
                if(ret)
                {
                    if(pPosition) // FIXME: Handle plugin aux input
                    {
                        auto& [refCount, connection] = connectToInputPositions_[pPosition];
                        if(!connection)
                        {
                            connection = QObject::connect(
                                pPosition, &QObject::destroyed,
                                this,
                                [this](QObject* obj) mutable
                                {
                                    FOR_RANGE0(i, mainInput_.size())
                                    {
                                        if(mainInput_[i] == obj)
                                        {
                                            setData(
                                                this->index(i),
                                                QVariant::fromValue<QObject*>(nullptr),
                                                Role::Input
                                            );
                                        }
                                    }
                                }
                            );
                        }
                        ++refCount;
                    }
                    else
                    {
                        auto oldPosition = mainInput_[row];
                        auto it = connectToInputPositions_.find(oldPosition);
                        if(it != connectToInputPositions_.end())
                        {
                            auto& [refCount, connection] = it->second;
                            if(--refCount == 0)
                            {
                                QObject::disconnect(connection);
                                connectToInputPositions_.erase(it);
                            }
                        }
                    }
                    mainInput_[row] = pPosition;
                    dataChanged(this->index(row), this->index(row), {Role::Input});
                }
            }
            return ret;
        }
        case Role::Output:
        {
            auto ret = false;
            if(listType_ == ListType::Regular)
            {
                auto pPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(value.value<QObject*>());
                if(!pPosition)
                {
                    ret = mixer_.setMainOutputAt(
                        row,
                        YADAW::Audio::Mixer::Mixer::Position {
                            YADAW::Audio::Mixer::Mixer::Position::Type::Invalid
                        }
                    );
                }
                else
                {
                    switch(pPosition->getType())
                    {
                    case YADAW::Entity::IAudioIOPosition::Type::AudioHardwareIOChannel:
                    {
                        const auto& hardwareAudioIOPosition = static_cast<const YADAW::Entity::HardwareAudioIOPosition&>(*pPosition);
                        ret = mixer_.setMainOutputAt(
                            row,
                            static_cast<YADAW::Audio::Mixer::Mixer::Position>(hardwareAudioIOPosition)
                        );
                        break;
                    }
                    case YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel:
                    {
                        const auto& regularAudioIOPosition = static_cast<const YADAW::Entity::RegularAudioIOPosition&>(*pPosition);
                        ret = mixer_.setMainOutputAt(
                            row,
                            static_cast<YADAW::Audio::Mixer::Mixer::Position>(regularAudioIOPosition)
                        );
                        break;
                    }
                    case YADAW::Entity::IAudioIOPosition::Type::PluginAuxIO:
                    {
                        const auto& pluginAuxIOPosition = static_cast<const YADAW::Entity::PluginAuxAudioIOPosition&>(*pPosition);
                        ret = mixer_.setMainOutputAt(
                            row,
                            static_cast<YADAW::Audio::Mixer::Mixer::Position>(pluginAuxIOPosition)
                        );
                        break;
                    }
                    }
                }
                if(ret)
                {
                    if(pPosition)
                    {
                        auto& [refCount, connection] = connectToOutputPositions_[pPosition];
                        if(!connection)
                        {
                            // output destination is about to be removed
                            connection = QObject::connect(
                                pPosition, &QObject::destroyed,
                                this,
                                [this](QObject* obj) mutable
                                {
                                    FOR_RANGE0(i, mainOutput_.size())
                                    {
                                        if(mainOutput_[i] == obj)
                                        {
                                            setData(
                                                this->index(i),
                                                QVariant::fromValue<QObject*>(nullptr),
                                                IMixerChannelListModel::Role::Output
                                            );
                                        }
                                    }
                                }
                            );
                        }
                        ++refCount;
                    }
                    else
                    {
                        auto oldPosition = mainOutput_[row];
                        auto it = connectToOutputPositions_.find(oldPosition);
                        if(it != connectToOutputPositions_.end())
                        {
                            auto& [refCount, connection] = it->second;
                            if(--refCount == 0)
                            {
                                QObject::disconnect(connection);
                                connectToOutputPositions_.erase(it);
                            }
                        }
                    }
                    mainOutput_[row] = pPosition;
                    dataChanged(this->index(row), this->index(row), {Role::Output});
                }
            }
            return ret;
        }
        case Role::InstrumentBypassed:
        {
            if(listType_ == ListType::Regular && mixer_.getInstrument(row) != nullptr)
            {
                mixer_.setInstrumentBypass(row, value.value<bool>());
                dataChanged(index, index, {Role::InstrumentBypassed});
                return true;
            }
            return false;
        }
        case Role::InstrumentWindowVisible:
        {
            if(listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
            {
                auto context = mixer_.getInstrumentContext(row);
                if(context.has_value())
                {
                    auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context->get().get());
                    if(auto editor = pluginContext.editor)
                    {
                        editor->setVisible(value.value<bool>());
                        return true;
                    }
                }
            }
            return false;
        }
        case Role::InstrumentGenericEditorVisible:
        {
            if(listType_ == ListType::Regular
                && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument)
            {
                auto context = mixer_.getInstrumentContext(row);
                if(context.has_value())
                {
                    auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context->get().get());
                    auto genericEditor = pluginContext.genericEditor;
                    genericEditor->setVisible(value.value<bool>());
                    return true;
                }
            }
            return false;
        }
        case Role::Mute:
        {
            auto& mute = (mixer_.*getMuteAt[listType_])(row)->get();
            mute.setMute(value.value<bool>());
            dataChanged(this->index(row), this->index(row), {Role::Mute});
            hasMuteChanged();
            return true;
        }
        case Role::Monitor:
        {
            auto ret = false;
            if(data(index, Role::MonitorExist).value<bool>())
            {
                auto multiInput = static_cast<YADAW::Audio::Engine::MultiInputDeviceWithPDC*>(
                    mixer_.graph().graph().getNodeData(
                        mixer_.channelPreFaderInsertsAt(row)->get().inNode()->inNodes().front()
                    ).process.device()
                );
                auto inputSwitcher = static_cast<YADAW::Audio::Util::InputSwitcher*>(
                    multiInput->process().device()
                );
                auto inputIndex = static_cast<std::uint32_t>(value.value<bool>());
                if(static_cast<bool>(inputSwitcher->getInputIndex()) != inputIndex)
                {
                    ret = inputSwitcher->setInputIndex(inputIndex);
                    if(ret)
                    {
                        dataChanged(
                            this->index(row), this->index(row),
                            { Role::Monitor }
                        );
                    }
                }
            }
            return ret;
        }
        case Role::Volume:
        {
            auto& fader = (mixer_.*getFader[listType_])(row)->get();
            if(!editingVolume_[row])
            {
                fader.beginEditGain();
                fader.performEditGain(value.value<double>());
                fader.endEditGain();
            }
            else
            {
                fader.performEditGain(value.value<double>());
            }
            dataChanged(this->index(row), this->index(row), {Role::Volume});
            return true;
        }
        case Role::EditingVolume:
        {
            auto& fader = (mixer_.*getFader[listType_])(row)->get();
            if(value.value<bool>())
            {
                fader.beginEditGain();
            }
            else
            {
                fader.endEditGain();
            }
            dataChanged(this->index(row), this->index(row), {Role::EditingVolume});
            return true;
        }
        }
    }
    return false;
}

decltype(&YADAW::Audio::Mixer::Mixer::insertAudioInputChannel) insertChannelsFunc[2] = {
    &YADAW::Audio::Mixer::Mixer::insertAudioOutputChannel,
    &YADAW::Audio::Mixer::Mixer::insertAudioInputChannel,
};

bool MixerChannelListModel::insert(int position, int count,
    IMixerChannelListModel::ChannelTypes type,
    YADAW::Entity::ChannelConfig::Config channelConfig,
    int channelCountInGroup)
{
    auto ret = false;
    if(listType_ == ListType::Regular)
    {
        ret = mixer_.insertChannels(position, count,
            channelTypeFromModelTypes(type),
            YADAW::Entity::groupTypeFromConfig(channelConfig),
            channelCountInGroup
        );
        if(ret)
        {
            FOR_RANGE(i, position, position + count)
            {
                mixer_.channelPreFaderInsertsAt(i)->get().setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
                mixer_.channelPostFaderInsertsAt(i)->get().setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
            }
            std::generate_n(
                std::inserter(insertModels_, insertModels_.begin() + position),
                count,
                [this, position, offset = 0]() mutable
                {
                    auto index = position + (offset++);
                    auto ret = std::make_unique<YADAW::Model::MixerChannelInsertListModel>(
                        mixer_.channelPreFaderInsertsAt(index)->get(),
                        listType_,
                        index,
                        true,
                        0
                    );
                    return ret;
                }
            );
            if(fillPluginContextCallback_ != &Impl::blankFillPluginContext)
            {
                FOR_RANGE(i, position, position + count)
                {
                    insertModels_[i]->setFillPluginContextCallback(fillPluginContextCallback_);
                }
            }
            FOR_RANGE(i, position + count, insertModels_.size())
            {
                insertModels_[i]->setChannelIndex(i);
            }
            updateInstrumentConnections(position + count);
            std::fill_n(
                std::inserter(
                    mainInput_, mainInput_.begin() + position
                ),
                count, nullptr
            );
            std::fill_n(
                std::inserter(
                    mainOutput_, mainOutput_.begin() + position
                ),
                count, nullptr
            );
            endInsertRows();
        }
    }
    else if(listType_ == ListType::AudioHardwareInput || listType_ == ListType::AudioHardwareOutput)
    {
        auto& audioBusConfiguration = YADAW::Controller::appAudioBusConfiguration();
        auto inNode = mixer_.graph().addNode(
            YADAW::Audio::Engine::AudioDeviceProcess(
                (listType_ == ListType::AudioHardwareInput?
                    audioBusConfiguration.getInputBusAt(position):
                    audioBusConfiguration.getOutputBusAt(position)
                )->get()
            )
        );
        (mixer_.*insertChannelsFunc[listType_ == ListType::AudioHardwareInput])(position, inNode, 0);
        auto& preFaderInserts = (
            listType_ == ListType::AudioHardwareInput?
                mixer_.audioInputChannelPreFaderInsertsAt(position):
                mixer_.audioOutputChannelPreFaderInsertsAt(position)
        )->get();
        auto& postFaderInserts = (
            listType_ == ListType::AudioHardwareInput?
                mixer_.audioInputChannelPostFaderInsertsAt(position):
                mixer_.audioOutputChannelPostFaderInsertsAt(position)
        )->get();
        preFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
        postFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
        insertModels_.emplace(
            insertModels_.begin() + position,
            std::make_unique<YADAW::Model::MixerChannelInsertListModel>(
                (
                    listType_ == ListType::AudioHardwareInput?
                    mixer_.audioInputChannelPreFaderInsertsAt(position):
                    mixer_.audioOutputChannelPreFaderInsertsAt(position)
                )->get(),
                listType_,
                position,
                true,
                0
            )
        );
        sendModels_.emplace(
            sendModels_.begin() + position,
            std::make_unique<YADAW::Model::MixerChannelSendListModel>(
                mixer_, listType_, position
            )
        );
        FOR_RANGE(i, position + 1, insertModels_.size())
        {
            insertModels_[i]->setChannelIndex(i);
            sendModels_[i]->setChannelIndex(i);
        }
        endInsertRows();
        dataChanged(index(position + count), index(itemCount() - 1), {Role::NameWithIndex});
        ret = true;
    }
    if(ret)
    {
        std::generate_n(
            std::inserter(polarityInverterModels_, polarityInverterModels_.begin() + position), count,
            [this, position, offset = 0]() mutable
            {
                auto index = position + (offset++);
                return std::make_unique<YADAW::Model::PolarityInverterModel>(
                    (mixer_.*getPolarityInverters[YADAW::Util::underlyingValue(listType_)])(index)->get()
                );
            }
        );
        std::generate_n(
            std::inserter(sendModels_, sendModels_.begin() + position), count,
            [this, position, offset = 0]() mutable
            {
                auto index = position + (offset++);
                return std::make_unique<MixerChannelSendListModel>(
                    mixer_, listType_, index
                );
            }
        );
        FOR_RANGE(i, position + count, insertModels_.size())
        {
            sendModels_[i]->setChannelIndex(i);
        }
        std::fill_n(
            std::inserter(editingVolume_, editingVolume_.begin() + position),
            count, false
        );
        if(position + count < itemCount())
        {
            dataChanged(
                index(position + count),
                index(itemCount() - 1),
                {Role::NameWithIndex}
            );
        }
    }
    return ret;
}

bool MixerChannelListModel::append(int count,
    IMixerChannelListModel::ChannelTypes type,
    YADAW::Entity::ChannelConfig::Config channelConfig,
    int channelCountInGroup)
{
    return insert(itemCount(), count, type, channelConfig, channelCountInGroup);
}

bool MixerChannelListModel::remove(int position, int removeCount)
{
    if(position < itemCount() && position + removeCount <= itemCount())
    {
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        editingVolume_.erase(
            editingVolume_.begin() + position,
            editingVolume_.begin() + position + removeCount
        );
        FOR_RANGE(i, position, position + removeCount)
        {
            insertModels_[i]->clear();
            removeInstrument(i); // TODO
        }
        if(listType_ == ListType::Regular)
        {
            FOR_RANGE0(i, mixer_.channelCount())
            {
                auto audioIOPosition = static_cast<YADAW::Entity::IAudioIOPosition*>(
                    data(
                        this->index(i),
                        IMixerChannelListModel::Role::Output
                    ).value<QObject*>()
                );
                if(audioIOPosition && audioIOPosition->getType() == YADAW::Entity::IAudioIOPosition::Type::BusAndFXChannel)
                {
                    auto regularAudioIOPosition = static_cast<YADAW::Entity::RegularAudioIOPosition*>(audioIOPosition);
                    auto sourceIndex = regularAudioIOPosition->getModel().getModel().mapToSource(regularAudioIOPosition->getIndex());
                    if(sourceIndex >= position && sourceIndex < position + removeCount)
                    {
                        setData(this->index(i), QVariant::fromValue<QObject*>(nullptr), IMixerChannelListModel::Role::Output);
                    }
                }
            }
            FOR_RANGE(i, position, position + removeCount)
            {
                setData(
                    this->index(i),
                    QVariant::fromValue<QObject*>(nullptr),
                    IMixerChannelListModel::Role::Output
                );
            }
        }
        (mixer_.*removeChannels[YADAW::Util::underlyingValue(listType_)])(
            position, removeCount
        );
        insertModels_.erase(
            insertModels_.begin() + position,
            insertModels_.begin() + position + removeCount
        );
        polarityInverterModels_.erase(
            polarityInverterModels_.begin() + position,
            polarityInverterModels_.begin() + position + removeCount
        );
        FOR_RANGE(i, position, insertModels_.size())
        {
            insertModels_[i]->setChannelIndex(i);
            sendModels_[i]->setChannelIndex(i);
        }
        if(listType_ == ListType::Regular)
        {
            mainInput_.erase(
                mainInput_.begin() + position,
                mainInput_.begin() + position + removeCount
            );
            mainOutput_.erase(
                mainOutput_.begin() + position,
                mainOutput_.begin() + position + removeCount
            );
            sendModels_.erase(
                sendModels_.begin() + position,
                sendModels_.begin() + position + removeCount
            );
            updateInstrumentConnections(position);
        }
        endRemoveRows();
        if(itemCount() != position)
        {
            dataChanged(index(position), index(itemCount() - 1), {Role::NameWithIndex});
        }
        return true;
    }
    return false;
}

bool MixerChannelListModel::move(int position, int moveCount, int newPosition)
{
    return false;
}

bool MixerChannelListModel::copy(int position, int copyCount, int newPosition)
{
    return false;
}

void MixerChannelListModel::clear()
{
    remove(0, itemCount());
}

bool MixerChannelListModel::setInstrument(int position, int pluginId)
{
    auto ret = false;
    ade::NodeHandle nodeHandle;
    if(listType_ == ListType::Regular
        && position >= 0
        && position < mixer_.channelCount()
        && mixer_.channelInfoAt(position)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument
    )
    {
        removeInstrument(position);
        const auto& pluginInfo = YADAW::DAO::selectPluginById(pluginId);
        YADAW::Controller::InitPluginArgs initPluginArgs;
        initPluginArgs.mainInputChannelGroup.first = YADAW::Audio::Base::ChannelGroupType::eInvalid;
        initPluginArgs.mainOutputChannelGroup = *mixer_.channelGroupTypeAndChannelCountAt(position);
        auto optionalPluginContext = YADAW::Controller::createPluginWithContext(
            pluginInfo.path,
            static_cast<YADAW::DAO::PluginFormat>(pluginInfo.format),
            pluginInfo.uid,
            [this, &initPluginArgs](YADAW::Controller::PluginContext& context)
            {
                return fillPluginContextCallback_(context, initPluginArgs);
            }
        );
        if(optionalPluginContext.has_value())
        {
            auto& pluginContext = *optionalPluginContext;
            pluginContext.position.position = YADAW::Controller::PluginPosition::InChannelPosition::Instrument;
            pluginContext.position.index = position;
            pluginContext.position.model = this;
            pluginContext.userData = YADAW::Util::createPMRUniquePtr(
                std::make_unique<PluginContextUserData>(
                    pluginContext, pluginInfo.name
                )
            );
            auto& plugin = pluginContext.pluginInstance.plugin()->get();
            auto& userData = *static_cast<PluginContextUserData*>(
                pluginContext.userData.get()
            );
            if(auto pluginWindow = pluginContext.editor)
            {
                pluginWindow->setTitle(pluginInfo.name);
                userData.pluginWindowConnection = QObject::connect(
                    pluginWindow, &QWindow::visibleChanged,
                    [this, position](bool visible)
                    {
                        dataChanged(this->index(position), this->index(position),
                            {Role::InstrumentWindowVisible}
                        );
                    }
                );
            }
            auto genericEditor = pluginContext.genericEditor;
            genericEditor->setTitle(pluginInfo.name);
            userData.genericEditorWindowConnection = QObject::connect(
                genericEditor, &QWindow::visibleChanged,
                [this, position](bool visible)
                {
                    dataChanged(this->index(position), this->index(position),
                        {Role::InstrumentGenericEditorVisible}
                    );
                }
            );
            genericEditor->setProperty(
                "parameterListModel",
                QVariant::fromValue<QObject*>(&userData.paramListModel)
            );
            std::uint32_t firstOutput = 0;
            FOR_RANGE0(i, plugin.audioOutputGroupCount())
            {
                auto& group = plugin.audioOutputGroupAt(i)->get();
                if(group.isMain() && group.type() == mixer_.channelGroupTypeAt(position))
                {
                    firstOutput = i;
                    break;
                }
            }
            auto node = pluginContext.node;
            auto mixerContext = YADAW::Util::createPMRUniquePtr(
                std::make_unique<YADAW::Controller::PluginContext>(
                    std::move(pluginContext)
                )
            );
            auto& engine = YADAW::Controller::AudioEngine::appAudioEngine();
            YADAW::Controller::appPluginContexts().emplace(mixerContext.get());
            mixer_.setInstrument(
                position,
                node,
                std::move(mixerContext),
                firstOutput
            );
            if(auto optionalBatchUpdater = mixer_.batchUpdater(); optionalBatchUpdater.has_value())
            {
                optionalBatchUpdater->get().addNull();
            }
            else
            {
                mixer_.connectionUpdated();
            }
            dataChanged(this->index(position), this->index(position),
                {
                    Role::InstrumentExist,
                    Role::InstrumentBypassed,
                    Role::InstrumentName,
                    Role::InstrumentAudioInputs,
                    Role::InstrumentAudioOutputs,
                    Role::InstrumentEventInputs,
                    Role::InstrumentEventOutputs,
                    Role::InstrumentHasUI,
                    Role::InstrumentWindowVisible,
                    Role::InstrumentGenericEditorVisible,
                    Role::InstrumentLatency
                }
            );
        }
    }
    YADAW::Controller::pluginWindows.pluginWindow = nullptr;
    YADAW::Controller::pluginWindows.genericEditorWindow = nullptr;
    return ret;
}

bool MixerChannelListModel::removeInstrument(int position)
{
    using YADAW::Audio::Plugin::IAudioPlugin;
    using YADAW::Audio::Mixer::Mixer;
    if(listType_ == ListType::Regular
       && position >= 0
       && position < mixer_.channelCount()
       && mixer_.channelInfoAt(position)->get().channelType == Mixer::ChannelType::Instrument
    )
    {
        auto optionalContext = mixer_.getInstrumentContext(position);
        if(!optionalContext.has_value())
        {
            return false;
        }
        auto& context = *static_cast<YADAW::Controller::PluginContext*>(optionalContext->get().get());
        if(!context.pluginInstance.plugin().has_value())
        {
            return false;
        }
        instrumentAboutToBeRemoved(position);
        auto& graphWithPDC = mixer_.graph();
        std::pair<ade::NodeHandle, YADAW::Audio::Mixer::Context> instrument = {ade::NodeHandle(), YADAW::Util::createUniquePtr(nullptr)};
        {
            instrument = mixer_.detachInstrument(position);
            auto deviceWithPDC = graphWithPDC.removeNode(instrument.first);
            mixer_.connectionUpdated();
        }
        YADAW::Controller::appPluginContexts().erase(&context);
        if(auto window = context.editor)
        {
            context.pluginInstance.plugin()->get().gui()->detachWithWindow();
            window->setProperty("destroyingPlugin", QVariant::fromValue(true));
            delete window;
        }
        auto genericEditor = context.genericEditor;
        genericEditor->setProperty("destroyingPlugin", QVariant::fromValue(true));
        delete genericEditor;
        auto& engine = YADAW::Controller::AudioEngine::appAudioEngine();
        auto plugin = &context.pluginInstance.plugin()->get();
        if(auto format = plugin->format(); format == YADAW::Audio::Plugin::PluginFormat::VST3)
        {
            auto& pool = YADAW::Controller::appVST3PluginPool();
            pool.erase(
                static_cast<YADAW::Audio::Plugin::VST3Plugin*>(plugin)
            );
            engine.vst3PluginPool().updateAndGetOld(
                std::make_unique<YADAW::Controller::VST3PluginPoolVector>(
                    YADAW::Controller::createPoolVector(pool)
                ),
                engine.running()
            );
        }
        else if(format == YADAW::Audio::Plugin::PluginFormat::CLAP)
        {
            // TODO: Decouple with `appAudioEngine`
            //   Since `CLAPPlugin::stopProcessing` has to be called in the
            //   audio thread, I have to couple this model with
            //   `appAudioEngine`.
            auto clapPlugin = static_cast<YADAW::Audio::Plugin::CLAPPlugin*>(plugin);
            auto& engine = YADAW::Controller::AudioEngine::appAudioEngine();
            engine.clapPluginToSetProcess().updateAndGetOld(
                std::make_unique<YADAW::Controller::CLAPPluginToSetProcessVector>(
                    1, std::make_pair(clapPlugin, false)
                ),
                engine.running()
            );
            auto& pool = YADAW::Controller::appCLAPPluginPool();
            pool.erase(
                static_cast<YADAW::Audio::Plugin::CLAPPlugin*>(plugin)
            );
            engine.clapPluginPool().updateAndGetOld(
                std::make_unique<YADAW::Controller::CLAPPluginPoolVector>(
                    YADAW::Controller::createPoolVector(pool)
                ),
                engine.running()
            );
        }
        instrument.second.reset();
        dataChanged(this->index(position), this->index(position),
            {
                Role::InstrumentExist,
                Role::InstrumentBypassed,
                Role::InstrumentName,
                Role::InstrumentAudioInputs,
                Role::InstrumentAudioOutputs,
                Role::InstrumentEventInputs,
                Role::InstrumentEventOutputs,
                Role::InstrumentHasUI,
                Role::InstrumentWindowVisible,
                Role::InstrumentGenericEditorVisible,
                Role::InstrumentLatency
            }
        );
        return true;
    }
    return false;
}

int MixerChannelListModel::getIndexOfId(const QString& id) const
{
    bool converted = false;
    auto idAsNum = id.toULongLong(&converted);
    if(converted)
    {
        auto getIndexResult =
            (mixer_.*getIndexOfIdFunc[YADAW::Util::underlyingValue(listType_)])(idAsNum);
        if(getIndexResult.has_value())
        {
            return *getIndexResult;
        }
    }
    return -1;
}

bool MixerChannelListModel::hasMute() const
{
    return (mixer_.*hasMuteFunc[listType_])();
}

void MixerChannelListModel::unmuteAll()
{
    (mixer_.*unmuteAllFunc[listType_])();
    dataChanged(index(0), index(itemCount() - 1), {Role::Mute});
    hasMuteChanged();
}

bool MixerChannelListModel::isComparable(int roleIndex) const
{
    return false;
}

bool MixerChannelListModel::isFilterable(int roleIndex) const
{
    constexpr auto filterableRoles = std::array {
        Role::Color,
        Role::ChannelType,
        Role::ChannelConfig,
        Role::ChannelCount,
        Role::InputExist,
        Role::InputType,
        Role::OutputExist,
        Role::OutputType,
        Role::InstrumentExist,
        Role::Mute,
        Role::Solo,
        Role::PolarityInverter,
        Role::ArmRecordingExist,
        Role::ArmRecording
    };
    return std::find(
        filterableRoles.begin(), filterableRoles.end(), roleIndex
    ) != filterableRoles.end();
}

bool MixerChannelListModel::isSearchable(int roleIndex) const
{
    return roleIndex == Role::Id
        || roleIndex == Role::Name;
}

bool MixerChannelListModel::isLess(int roleIndex,
    const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return false;
}

bool MixerChannelListModel::isSearchPassed(int role,
    const QModelIndex& modelIndex,
    const QString& string, Qt::CaseSensitivity caseSensitivity) const
{
    if(auto row = modelIndex.row(); row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Id:
            return data(modelIndex, role).value<QString>().contains(string);
        case Role::Name:
            return data(modelIndex, role).value<QString>().contains(
                string, caseSensitivity
            );
        }
    }
    return false;
}

bool MixerChannelListModel::isPassed(const QModelIndex& modelIndex,
    int role, const QVariant& variant) const
{
    if(auto row = modelIndex.row(); row >= 0 && row < itemCount())
    {
        auto sourceData = data(modelIndex, role);
        switch(role)
        {
        case Role::Color:
            return sourceData.value<QColor>() == variant.value<QColor>();
        case Role::ChannelType:
            return sourceData.value<ChannelTypes>() == variant.value<ChannelTypes>();
        case Role::ChannelConfig:
            return sourceData.value<YADAW::Entity::ChannelConfig::Config>() == variant.value<YADAW::Entity::ChannelConfig::Config>();
        case Role::ChannelCount:
            return sourceData.value<int>() == variant.value<int>();
        case Role::InputExist:
            return sourceData.value<bool>() == variant.value<bool>();
        case Role::InputType:
            return sourceData.value<MediaTypes>() == variant.value<MediaTypes>();
        case Role::OutputExist:
            return sourceData.value<bool>() == variant.value<bool>();
        case Role::OutputType:
            return sourceData.value<MediaTypes>() == variant.value<MediaTypes>();
        case Role::InstrumentExist:
            return sourceData.value<bool>() == variant.value<bool>();
        case Role::Mute:
            return sourceData.value<bool>() == variant.value<bool>();
        case Role::Solo:
            return sourceData.value<bool>() == variant.value<bool>();
        case Role::PolarityInverter:
            return sourceData.value<bool>() == variant.value<bool>();
        case Role::ArmRecordingExist:
            return sourceData.value<bool>() == variant.value<bool>();
        case Role::ArmRecording:
            return sourceData.value<bool>() == variant.value<bool>();
        }
    }
    return false;
}

MixerChannelListModel::ListType MixerChannelListModel::type() const
{
    return listType_;
}

YADAW::Audio::Mixer::Mixer& MixerChannelListModel::mixer()
{
    return mixer_;
}

const YADAW::Audio::Mixer::Mixer& MixerChannelListModel::mixer() const
{
    return mixer_;
}

void MixerChannelListModel::setFillPluginContextCallback(
    FillPluginContextCallback* callback)
{
    fillPluginContextCallback_ = callback;
    for(auto& insertModel: insertModels_)
    {
        insertModel->setFillPluginContextCallback(callback);
    }
}

void MixerChannelListModel::resetFillPluginContextCallback()
{
    fillPluginContextCallback_ = &Impl::blankFillPluginContext;
    for(auto& insertModel: insertModels_)
    {
        insertModel->resetFillPluginContextCallback();
    }
}

void MixerChannelListModel::instrumentLatencyUpdated(std::uint32_t index) const
{
    if(listType_ == ListType::Regular
        && index < itemCount()
        && mixer_.channelInfoAt(index)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument
    )
    {
        const_cast<MixerChannelListModel*>(this)->dataChanged(
            this->index(index),
            this->index(index),
            {Role::InstrumentLatency}
        );
    }
}

void MixerChannelListModel::updateInstrumentIOConfig(std::uint32_t index)
{
    if(listType_ == ListType::Regular
        && index < itemCount()
        && mixer_.channelInfoAt(index)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument
    )
    {
        auto& context = mixer_.getInstrumentContext(index)->get();
        auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(context.get());
        auto& pluginContextUserData = *static_cast<PluginContextUserData*>(
            pluginContext.userData.get()
        );
        auto& graphWithPDC = mixer_.graph();
        auto& graph = graphWithPDC.graph();
        auto instrumentNode = pluginContext.node;
        struct Point
        {
            std::uint32_t fromChannelGroupIndex;
            std::uint32_t toChannelGroupIndex;
            ade::NodeHandle node;
        };
        std::vector<Point> inputPoints;
        std::vector<Point> outputPoints;
        auto inEdges = instrumentNode->inEdges();
        auto size = inEdges.size();
        inputPoints.reserve(size);
        for(const auto& edgeHandle: inEdges)
        {
            const auto& edgeData = graph.getEdgeData(edgeHandle);
            inputPoints.emplace_back(
                Point {
                    edgeData.fromChannel,
                    edgeData.toChannel,
                    edgeHandle->srcNode()
                }
            );
        }
        auto outEdges = instrumentNode->outEdges();
        size = outEdges.size();
        outputPoints.reserve(size);
        for(const auto& edgeHandle: outEdges)
        {
            const auto& edgeData = graph.getEdgeData(edgeHandle);
            outputPoints.emplace_back(
                Point {
                    edgeData.fromChannel,
                    edgeData.toChannel,
                    edgeHandle->dstNode()
                }
            );
        }
        auto instrumentIsBypassed = mixer_.isInstrumentBypassed(index);
        setData(this->index(index), QVariant::fromValue<bool>(true),
            IMixerChannelListModel::Role::InstrumentBypassed
        );
        auto process = graph.getNodeData(instrumentNode).process;
        auto device = process.device();
        if(device->audioInputGroupCount() > 1)
        {
            auto multiInput = static_cast<YADAW::Audio::Engine::MultiInputDeviceWithPDC*>(device);
            process = multiInput->process();
            device = process.device();
        }
        {
            auto disposingDevice = graphWithPDC.removeNode(instrumentNode);
            YADAW::Controller::AudioEngine::mixerConnectionUpdatedCallback(mixer_);
        }
        auto plugin = static_cast<YADAW::Audio::Plugin::IAudioPlugin*>(device);
        auto status = plugin->status();
        // TODO: Start/stop processing plugins on audio thread if needed
        plugin->stopProcessing();
        plugin->deactivate();
        pluginContextUserData.audioInputs.reset();
        pluginContextUserData.audioOutputs.reset();
        if(status >= YADAW::Audio::Plugin::IAudioPlugin::Status::Activated)
        {
            plugin->activate();
        }
        if(status >= YADAW::Audio::Plugin::IAudioPlugin::Status::Processing)
        {
            // TODO: Start/stop processing plugins on audio thread if needed
            plugin->startProcessing();
        }
        pluginContext.node = graphWithPDC.addNode(process);
        // TODO: Restore additional connections
        setData(this->index(index), QVariant::fromValue<bool>(instrumentIsBypassed),
            IMixerChannelListModel::Role::InstrumentBypassed
        );
    }
}

void MixerChannelListModel::updateInstrumentConnections(std::uint32_t from)
{
    FOR_RANGE(i, from, itemCount())
    {
        auto optionalContext = mixer_.getInstrumentContext(i);
        if(optionalContext.has_value())
        {
            auto& pluginContext = *static_cast<YADAW::Controller::PluginContext*>(optionalContext->get().get());
            auto& pluginContextUserData = *static_cast<PluginContextUserData*>(pluginContext.userData.get());
            if(pluginContext.editor)
            {
                QObject::disconnect(
                    pluginContextUserData.pluginWindowConnection
                );
                pluginContextUserData.pluginWindowConnection = QObject::connect(
                    pluginContext.editor,
                    &QWindow::visibleChanged,
                    [this, i](bool visible)
                    {
                        dataChanged(this->index(i), this->index(i),
                            {Role::InstrumentWindowVisible}
                        );
                    }
                );
            }
            QObject::disconnect(
                pluginContextUserData.genericEditorWindowConnection
            );
            pluginContextUserData.genericEditorWindowConnection = QObject::connect(
                pluginContext.genericEditor,
                &QWindow::visibleChanged,
                [this, i](bool visible)
                {
                    dataChanged(this->index(i), this->index(i),
                        {Role::InstrumentGenericEditorVisible}
                    );
                }
            );
            pluginContext.position.index = i;
        }
    }
}
}
