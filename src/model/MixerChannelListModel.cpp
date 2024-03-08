#include "MixerChannelListModel.hpp"

#include "audio/plugin/CLAPPlugin.hpp"
#include "audio/plugin/VST3Plugin.hpp"
#include "audio/plugin/VestifalPlugin.hpp"
#include "controller/AudioEngineController.hpp"
#include "controller/AudioGraphBackendController.hpp"
#include "controller/ALSABackendController.hpp"
#include "controller/PluginController.hpp"
#include "controller/PluginLatencyUpdatedCallback.hpp"
#include "controller/PluginWindowController.hpp"
#include "dao/PluginTable.hpp"
#include "entity/ChannelConfigHelper.hpp"
#include "event/EventBase.hpp"
#include "model/MixerChannelInsertListModel.hpp"
#include "util/Base.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
struct InstrumentInstance
{
    YADAW::Audio::Plugin::IAudioPlugin* plugin;
    ade::NodeHandle instrumentNode;
    YADAW::UI::WindowAndConnection pluginWindowConnection;
    YADAW::UI::WindowAndConnection genericEditorWindowConnection;
    YADAW::Model::PluginParameterListModel paramListModel;
    YADAW::Controller::LibraryPluginMap::iterator libraryPluginIterator = {};
    YADAW::Controller::PluginContextMap::iterator pluginContextIterator = {};
    QString name;
    template<typename T>
    InstrumentInstance(YADAW::Audio::Mixer::Mixer& mixer, T* plugin):
        plugin(plugin),
        pluginWindowConnection(nullptr),
        genericEditorWindowConnection(nullptr),
        paramListModel(*(plugin->parameter()))
    {}
};

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

using GetMute =
    std::optional<bool>(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t) const;

GetMute getMute[3] = {
    &YADAW::Audio::Mixer::Mixer::audioInputMuted,
    &YADAW::Audio::Mixer::Mixer::muted,
    &YADAW::Audio::Mixer::Mixer::audioOutputMuted
};

using SetMute =
    void(YADAW::Audio::Mixer::Mixer::*)(std::uint32_t, bool);

SetMute setMute[3] = {
    &YADAW::Audio::Mixer::Mixer::setAudioInputMuted,
    &YADAW::Audio::Mixer::Mixer::setMuted,
    &YADAW::Audio::Mixer::Mixer::setAudioOutputMuted
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

IMixerChannelListModel::ChannelTypes getChannelType(YADAW::Audio::Mixer::Mixer::ChannelType type)
{
    return static_cast<IMixerChannelListModel::ChannelTypes>(YADAW::Util::underlyingValue(type));
}

MixerChannelListModel::MixerChannelListModel(
    YADAW::Audio::Mixer::Mixer& mixer, ListType listType, QObject* parent):
    IMixerChannelListModel(parent),
    mixer_(mixer),
    listType_(listType)
{
    auto count = itemCount();
    insertModels_.reserve(count);
    FOR_RANGE0(i, count)
    {
        insertModels_.emplace_back(
            std::make_unique<YADAW::Model::MixerChannelInsertListModel>(
                (mixer_.*getPreFaderInserts[YADAW::Util::underlyingValue(listType_)])(i)->get(),
                listType_,
                i,
                true,
                0
            )
        );
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
        auto isInstrument =
            listType_ == ListType::Regular
            && mixer_.channelInfoAt(row)->get().channelType == YADAW::Audio::Mixer::Mixer::ChannelType::Instrument;
        switch(role)
        {
        case Role::Name:
        {
            const auto& optionalInfo = (mixer_.*getConstChannelInfo[YADAW::Util::underlyingValue(listType_)])(row);
            if(optionalInfo.has_value())
            {
                return QVariant::fromValue(optionalInfo->get().name);
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
        case Role::InstrumentExist:
        {
            return QVariant::fromValue(static_cast<bool>(instruments_[row]));
        }
        case Role::InstrumentBypassed:
        {
            if(instruments_[row])
            {
                return QVariant::fromValue(instrumentBypassed_[row]);
            }
            return {};
        }
        case Role::InstrumentName:
        {
            if(instruments_[row])
            {
                return QVariant::fromValue(instruments_[row]->name);
            }
            return {};
        }
        case Role::InstrumentHasUI:
        {
            if(instruments_[row])
            {
                auto ret = static_cast<bool>(instruments_[row]->plugin->gui());
                return QVariant::fromValue<bool>(ret);
            }
            return {};
        }
        case Role::InstrumentWindowVisible:
        {
            if(instruments_[row])
            {
                if(auto window = instruments_[row]->pluginWindowConnection.window; window)
                {
                    auto ret = window->isVisible();
                    return QVariant::fromValue(ret);
                }
            }
            return {};
        }
        case Role::InstrumentGenericEditorVisible:
        {
            if(instruments_[row])
            {
                auto window = instruments_[row]->genericEditorWindowConnection.window;
                return QVariant::fromValue(window->isVisible());
            }
            return {};
        }
        case Role::InstrumentLatency:
        {
            if(instruments_[row])
            {
                return QVariant::fromValue(
                    instruments_[row]->plugin->latencyInSamples()
                );
            }
            return {};
        }
        case Role::Inserts:
        {
            return QVariant::fromValue<QObject*>(insertModels_[row].get());
        }
        case Role::Mute:
        {
            return QVariant::fromValue<bool>(
                *(mixer_.*getMute[YADAW::Util::underlyingValue(listType_)])(row)
            );
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
                dataChanged(index, index, {Role::Name});
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
        case Role::InstrumentWindowVisible:
        {
            if(instruments_[row])
            {
                if(auto window = instruments_[row]->pluginWindowConnection.window; window)
                {
                    window->setVisible(value.value<bool>());
                    dataChanged(this->index(row), this->index(row), {Role::InstrumentWindowVisible});
                    return true;
                }
            }
            return false;
        }
        case Role::InstrumentGenericEditorVisible:
        {
            if(instruments_[row])
            {
                auto window = instruments_[row]->genericEditorWindowConnection.window;
                window->setVisible(value.value<bool>());
                dataChanged(this->index(row), this->index(row), {Role::InstrumentGenericEditorVisible});
                return true;
            }
            return false;
        }
        case Role::Mute:
        {
            (mixer_.*setMute[YADAW::Util::underlyingValue(listType_)])(row, value.value<bool>());
            dataChanged(index, index, {Role::Mute});
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

bool MixerChannelListModel::insert(int position, IMixerChannelListModel::ChannelTypes type,
    YADAW::Entity::ChannelConfig::Config channelConfig,
    int channelCountInGroup)
{
    if(listType_ == ListType::Regular)
    {
        auto ret = mixer_.insertChannel(position, channelTypeFromModelTypes(type),
            YADAW::Entity::groupTypeFromConfig(channelConfig),
            channelCountInGroup
        );
        if(ret)
        {
            auto& preFaderInserts = mixer_.channelPreFaderInsertsAt(position)->get();
            auto& postFaderInserts = mixer_.channelPostFaderInsertsAt(position)->get();
            preFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
            preFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
            preFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
            postFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
            postFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
            postFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
            beginInsertRows(QModelIndex(), position, position);
            insertModels_.emplace(
                insertModels_.begin() + position,
                std::make_unique<YADAW::Model::MixerChannelInsertListModel>(
                    mixer_.channelPreFaderInsertsAt(position)->get(),
                    listType_,
                    position,
                    true,
                    0
                )
            );
            instruments_.emplace(
                instruments_.begin() + position,
                std::unique_ptr<InstrumentInstance>(nullptr)
            );
            instrumentBypassed_.emplace(
                instrumentBypassed_.begin() + position, false
            );
            FOR_RANGE(i, position + 1, insertModels_.size())
            {
                insertModels_[i]->setChannelIndex(i);
            }
            endInsertRows();
        }
        return ret;
    }
    else if(listType_ == ListType::AudioHardwareInput || listType_ == ListType::AudioHardwareOutput)
    {
        auto& audioBusConfiguration = YADAW::Controller::appAudioBusConfiguration();
        beginInsertRows(QModelIndex(), position, position);
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
        preFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
        preFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
        preFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
        postFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
        postFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
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
        FOR_RANGE(i, position + 1, insertModels_.size())
        {
            insertModels_[i]->setChannelIndex(i);
        }
        endInsertRows();
        return true;
    }
    return false;
}

bool MixerChannelListModel::append(IMixerChannelListModel::ChannelTypes type,
    YADAW::Entity::ChannelConfig::Config channelConfig,
    int channelCountInGroup)
{
    return insert(itemCount(), type, channelConfig, channelCountInGroup);
}

bool MixerChannelListModel::remove(int position, int removeCount)
{
    auto& audioEngine = YADAW::Controller::AudioEngine::appAudioEngine();
    if(position < itemCount() && position + removeCount <= itemCount())
    {
        beginRemoveRows(QModelIndex(), position, position + removeCount - 1);
        FOR_RANGE(i, position, position + removeCount)
        {
            insertModels_[i]->clear();
            removeInstrument(i);
        }
        (mixer_.*removeChannels[YADAW::Util::underlyingValue(listType_)])(
            position, removeCount
        );
        insertModels_.erase(
            insertModels_.begin() + position,
            insertModels_.begin() + position + removeCount
        );
        FOR_RANGE(i, position, insertModels_.size())
        {
            insertModels_[i]->setChannelIndex(i);
        }
        endRemoveRows();
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
    using PluginFormat = YADAW::DAO::PluginFormat;
    using YADAW::Audio::Mixer::Mixer;
    auto ret = false;
    ade::NodeHandle nodeHandle;
    if(listType_ == ListType::Regular
        && position >= 0
        && position < mixer_.channelCount()
        && mixer_.channelInfoAt(position)->get().channelType == Mixer::ChannelType::Instrument
    )
    {
        auto& engine = YADAW::Controller::AudioEngine::appAudioEngine();
        auto& graphWithPDC = mixer_.graph();
        auto& libraryPluginMap = YADAW::Controller::appLibraryPluginMap();
        const auto& pluginInfo = YADAW::DAO::selectPluginById(pluginId);
        auto optionalCreatePluginResult = YADAW::Controller::createPlugin(
            pluginInfo.path,
            static_cast<PluginFormat>(pluginInfo.format),
            pluginInfo.uid
        );
        if(!optionalCreatePluginResult.has_value())
        {
            return false;
        }
        auto& [it, pluginAndProcess] = *optionalCreatePluginResult;
        auto& [plugin, process] = pluginAndProcess;
        switch(pluginInfo.format)
        {
        case PluginFormat::PluginFormatVST3:
        {
            auto pluginPtr = static_cast<YADAW::Audio::Plugin::VST3Plugin*>(plugin);
            auto vst3ComponentHandler = new(std::nothrow) YADAW::Audio::Host::VST3ComponentHandler(*pluginPtr);
            if(vst3ComponentHandler)
            {
                vst3ComponentHandler->setLatencyChangedCallback(
                    &YADAW::Controller::latencyUpdated<YADAW::Audio::Plugin::VST3Plugin>
                );
                pluginPtr->initialize(engine.sampleRate(), engine.bufferSize());
                auto inputCount = pluginPtr->audioInputGroupCount();
                auto outputCount = pluginPtr->audioOutputGroupCount();
                auto channelGroupType = *(mixer_.channelGroupType(position));
                std::vector<YADAW::Audio::Base::ChannelGroupType> inputChannels(
                    inputCount, channelGroupType
                );
                std::vector<YADAW::Audio::Base::ChannelGroupType> outputChannels(
                    outputCount, channelGroupType
                );
                pluginPtr->setChannelGroups(
                    inputChannels.data(), inputChannels.size(),
                    outputChannels.data(), outputChannels.size()
                );
                pluginPtr->activate();
                pluginPtr->startProcessing();
                auto& vst3PluginPool = YADAW::Controller::appVST3PluginPool();
                vst3PluginPool.emplace(
                    pluginPtr,
                    YADAW::Controller::VST3PluginContext{vst3ComponentHandler}
                );
                nodeHandle = graphWithPDC.addNode(std::move(process));
                engine.vst3PluginPool().updateAndDispose(
                    std::make_unique<YADAW::Controller::VST3PluginPoolVector>(
                        createPoolVector(vst3PluginPool)
                    ),
                    engine.running()
                );
                ret = true;
                break;
            }
        }
        case PluginFormat::PluginFormatCLAP:
        {
            auto pluginPtr = static_cast<YADAW::Audio::Plugin::CLAPPlugin*>(plugin);
            pluginPtr->host().setLatencyChangedCallback(
                &YADAW::Controller::latencyUpdated<YADAW::Audio::Plugin::CLAPPlugin>
            );
            YADAW::Audio::Host::CLAPHost::setMainThreadId(std::this_thread::get_id());
            pluginPtr->initialize(engine.sampleRate(), engine.bufferSize());
            pluginPtr->activate();
            auto clapEventList = new(std::nothrow) YADAW::Audio::Host::CLAPEventList();
            if(clapEventList)
            {
                clapEventList->attachToProcessData(pluginPtr->processData());
            }
            auto& clapPluginPool = YADAW::Controller::appCLAPPluginPool();
            clapPluginPool.emplace(
                pluginPtr,
                YADAW::Controller::CLAPPluginContext{clapEventList}
            );
            nodeHandle = graphWithPDC.addNode(std::move(process));
            engine.clapPluginToSetProcess().update(
                std::make_unique<YADAW::Controller::CLAPPluginToSetProcessVector>(
                    1, std::make_pair(pluginPtr, true)
                ),
                engine.running()
            );
            engine.clapPluginPool().update(
                std::make_unique<YADAW::Controller::CLAPPluginPoolVector>(
                    createPoolVector(clapPluginPool)
                ),
                engine.running()
            );
            engine.clapPluginPool().disposeOld(engine.running());
            engine.clapPluginToSetProcess().disposeOld(engine.running());
            ret = true;
            break;
        }
        case PluginFormat::PluginFormatVestifal:
        {
            ret = false;
            break;
        }
        }
        if(ret)
        {
            auto& instrument = instruments_[position];
            instrument = std::make_unique<InstrumentInstance>(
                mixer_, plugin
            );
            instrument->instrumentNode = nodeHandle;
            instrument->name = pluginInfo.name;
            instrument->libraryPluginIterator = it;
            YADAW::Controller::pluginNeedsWindow = plugin;
            if(plugin->gui())
            {
                YADAW::Event::eventHandler->createPluginWindow();
            }
            YADAW::Event::eventHandler->createGenericPluginEditor();
            auto& [pluginWindow, genericEditor] = YADAW::Controller::pluginWindows;
            if(pluginWindow)
            {
                pluginWindow->setTitle(pluginInfo.name);
                instrument->pluginWindowConnection.window = pluginWindow;
                instrument->pluginWindowConnection.connection = QObject::connect(
                    pluginWindow, &QWindow::visibleChanged,
                    [this, position](bool visible)
                    {
                        dataChanged(this->index(position), this->index(position),
                            {Role::InstrumentWindowVisible}
                        );
                    }
                );
            }
            genericEditor->setTitle(pluginInfo.name);
            instrument->genericEditorWindowConnection.window = genericEditor;
            instrument->genericEditorWindowConnection.connection = QObject::connect(
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
                QVariant::fromValue<QObject*>(&instrument->paramListModel)
            );
            FOR_RANGE(i, position + 1, instruments_.size())
            {
                if(auto instrument = instruments_[i].get(); instrument)
                {
                    if(auto& pluginWindowConnection = instrument->pluginWindowConnection;
                        pluginWindowConnection.window)
                    {
                        QObject::disconnect(
                            pluginWindowConnection.connection
                        );
                        pluginWindowConnection.connection = QObject::connect(
                            pluginWindowConnection.window,
                            &QWindow::visibleChanged,
                            [this, i](bool visible)
                            {
                                dataChanged(this->index(i), this->index(i),
                                    {Role::InstrumentWindowVisible}
                                );
                            }
                        );
                    }
                    auto& genericEditorWindowConnection =
                        instrument->genericEditorWindowConnection;
                    QObject::disconnect(
                        genericEditorWindowConnection.connection
                    );
                    genericEditorWindowConnection.connection = QObject::connect(
                        genericEditorWindowConnection.window,
                        &QWindow::visibleChanged,
                        [this, i](bool visible)
                        {
                            dataChanged(this->index(i), this->index(i),
                                {Role::InstrumentGenericEditorVisible}
                            );
                        }
                    );
                }
            }
            dataChanged(this->index(position), this->index(position),
                {
                    Role::InstrumentWindowVisible,
                    Role::InstrumentGenericEditorVisible,
                    Role::InstrumentHasUI
                }
            );
            dataChanged(this->index(position), this->index(position),
                {
                    Role::InstrumentExist,
                    Role::InstrumentBypassed,
                    Role::InstrumentName,
                    Role::InstrumentAudioInputs,
                    Role::InstrumentAudioOutputs,
                    Role::InstrumentEventInputs,
                    Role::InstrumentEventOutputs,
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
    using YADAW::Audio::Mixer::Mixer;
    if(listType_ == ListType::Regular
       && position >= 0
       && position < mixer_.channelCount()
       && mixer_.channelInfoAt(position)->get().channelType == Mixer::ChannelType::Instrument
    )
    {
        auto& graph = mixer_.graph();
        auto& instrumentInstance = instruments_[position];
        if(!instrumentInstance)
        {
            return false;
        }
        graph.removeNode(instrumentInstance->instrumentNode);
        auto& engine = YADAW::Controller::AudioEngine::appAudioEngine();
        engine.processSequence().updateAndDispose(
            std::make_unique<YADAW::Audio::Engine::ProcessSequence>(
                YADAW::Audio::Engine::getProcessSequence(graph.graph(), mixer_.bufferExtension())
            )
        );
        if(auto window = instrumentInstance->pluginWindowConnection.window; window)
        {
            delete window;
        }
        delete instrumentInstance->genericEditorWindowConnection.window;
        auto& pluginPool = instrumentInstance->libraryPluginIterator->second;
        auto it = pluginPool.find(
            instrumentInstance->plugin
        );
        if(it != pluginPool.end())
        {
            pluginPool.erase(it);
            if(pluginPool.empty())
            {
                YADAW::Controller::appLibraryPluginMap().erase(instrumentInstance->libraryPluginIterator);
            }
        }
        return true;
    }
    return false;
}
}