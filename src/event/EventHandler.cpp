#include "EventHandler.hpp"

#include "audio/host/CLAPHost.hpp"
#include "base/Constants.hpp"
#include "controller/AppController.hpp"
#include "controller/AssetDirectoryController.hpp"
#include "controller/AudioBackendController.hpp"
#include "controller/AudioBusConfigurationController.hpp"
#include "controller/AudioEngineController.hpp"
#include "controller/ConfigController.hpp"
#include "controller/GeneralSettingsController.hpp"
#include "controller/LocalizationController.hpp"
#include "controller/MixerChannelListModelController.hpp"
#include "controller/PluginController.hpp"
#include "controller/PluginDirectoryController.hpp"
#include "controller/PluginListController.hpp"
#include "entity/ChannelConfigHelper.hpp"
#include "event/EventBase.hpp"
#include "model/HardwareAudioIOPositionModel.hpp"
#include "model/MixerChannelListModel.hpp"
#include "model/NativePopupEventFilterModel.hpp"
#include "model/RegularAudioIOPositionModel.hpp"
#include "native/Native.hpp"
#include "ui/MessageDialog.hpp"
#include "ui/Runtime.hpp"
#include "ui/UI.hpp"
#include "util/IntegerRange.hpp"
#if _WIN32
#include "controller/AudioGraphBackendController.hpp"
#elif __linux__
#include "audio/host/VST3RunLoop.hpp"
#include "audio/host/EventFileDescriptorSupport.hpp"
#include "controller/ALSABackendController.hpp"
#endif

#include <QDir>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtCore/private/qconfig_p.h>
// `QT_COPYRIGHT_YEAR` is a private macro added since Qt 6.5.1
// Define this macro as the year of now since it's absent from Qt 6.5.0 and
// previous versions
#ifndef QT_COPYRIGHT_YEAR
#define QT_COPYRIGHT_YEAR "2023"
#endif

#include <algorithm>
#include <deque>
#include <thread>

namespace YADAW::Event
{
void saveAudioBusConfiguration(
    const YADAW::Audio::Device::IAudioBusConfiguration& configuration,
    const YADAW::Model::AudioBusConfigurationModel& inputModel,
    const YADAW::Model::AudioBusConfigurationModel& outputModel)
{
    using namespace YADAW::Controller;
    auto config = loadConfig();
    config["audio-bus"] = exportFromAudioBusConfiguration(
            configuration, inputModel, outputModel
    );
    saveConfig(config);
}

void saveAudioBackendState()
{
    auto appConfig = YADAW::Controller::loadConfig();
#if _WIN32
    const auto& audioGraphConfig = YADAW::Controller::deviceConfigFromCurrentAudioGraph();
    appConfig["audio-hardware"]["audiograph"] = audioGraphConfig;
    YADAW::Controller::saveConfig(appConfig);
#elif __linux__
    const auto& alsaConfig = YADAW::Controller::deviceConfigFromALSA();
    appConfig["audio-hardware"]["alsa"] = alsaConfig;
    YADAW::Controller::saveConfig(appConfig);
#endif
}

EventHandler::EventHandler(QObject* sender, QObject* receiver, QObject* parent):
    QObject(parent), eventSender_(sender), eventReceiver_(receiver)
{
    connectToEventSender(sender);
    connectToEventReceiver(receiver);
}

void EventHandler::connectToEventSender(QObject* sender)
{
    QObject::connect(sender, SIGNAL(startInitializingApplication()),
        this, SLOT(onStartInitializingApplication()));
    QObject::connect(sender, SIGNAL(mainWindowClosing()),
        this, SLOT(onMainWindowClosing()));
    QObject::connect(sender, SIGNAL(locatePathInExplorer(QString)),
        this, SLOT(onLocateFileInExplorer(QString)));
    QObject::connect(sender, SIGNAL(startPluginScan()),
        this, SLOT(onStartPluginScan()));
    QObject::connect(sender, SIGNAL(setMainWindowFromMaximizedToFullScreen()),
        this, SLOT(onSetMainWindowFromMaximizedToFullScreen()));
    QObject::connect(sender, SIGNAL(setMainWindowFromFullScreenToMaximized()),
        this, SLOT(onSetMainWindowFromFullScreenToMaximized()));
}

void EventHandler::connectToEventReceiver(QObject* receiver)
{
    QObject::connect(this, SIGNAL(mainWindowReady()),
        receiver, SIGNAL(mainWindowReady()));
    QObject::connect(this, SIGNAL(mainWindowCloseAccepted()),
        receiver, SIGNAL(mainWindowClosingAccepted()));
    QObject::connect(this, SIGNAL(setQtVersion(int, int, int)),
        receiver, SIGNAL(setQtVersion(int, int, int)));
    QObject::connect(this, SIGNAL(setQtCopyrightYear(QString)),
        receiver, SIGNAL(setQtCopyrightYear(QString)));
    QObject::connect(this, SIGNAL(setSplashScreenText(QString)),
        receiver, SIGNAL(setSplashScreenText(QString)));
    QObject::connect(this, SIGNAL(pluginScanComplete()),
        receiver, SIGNAL(pluginScanComplete()));
    QObject::connect(this, SIGNAL(setPluginScanTotalCount(int)),
        receiver, SIGNAL(setPluginScanTotalCount(int)),
        Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, SIGNAL(setScanningDirectories(bool)),
        receiver, SIGNAL(setScanningDirectories(bool)),
        Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, SIGNAL(setPluginScanProgress(int, QString)),
        receiver, SIGNAL(setPluginScanProgress(int, QString)),
        Qt::ConnectionType::QueuedConnection);
}

void EventHandler::onStartInitializingApplication()
{
    YADAW::Event::splashScreenWorkerThread->setText("Initializing application...");
    YADAW::Controller::initApplication();
    YADAW::Event::splashScreenWorkerThread->setText("Opening main window...");
    YADAW::Event::splashScreenWorkerThread->openMainWindow();
}

void EventHandler::onOpenMainWindow()
{
    auto appConfig = YADAW::Controller::loadConfig();
    auto currentBackend = YADAW::Controller::backendFromConfig(appConfig["audio-hardware"]);
    // -------------------------------------------------------------------------
    // initialize audio backend ------------------------------------------------
    // -------------------------------------------------------------------------
#if _WIN32
    auto& backend = YADAW::Controller::appAudioGraphBackend();
    auto audioGraphNode = appConfig["audio-hardware"]["audiograph"];
    YADAW::Native::ErrorCodeType errorCode = ERROR_SUCCESS;
    if(audioGraphNode.IsNull())
    {
        YADAW::Controller::saveConfig(appConfig);
        backend.initialize();
        errorCode = YADAW::Controller::activateDefaultDevice(backend);
        appConfig["audio-hardware"]["audiograph"] = YADAW::Controller::deviceConfigFromCurrentAudioGraph();
    }
    else
    {
        errorCode = YADAW::Controller::createAudioGraphFromConfig(audioGraphNode);
    }
    if(errorCode != ERROR_SUCCESS)
    {
        auto errorString = YADAW::Audio::Backend::getAudioGraphErrorStringFromErrorCode(errorCode);
        YADAW::UI::createMessageDialog();
        YADAW::UI::setHideCloseButton(true);
        auto messageDialog = YADAW::UI::messageDialog;
        if(messageDialog)
        {
            messageDialog->setProperty(
                "icon",
                QVariant::fromValue<int>(YADAW::UI::IconType::Warning)
            );
            messageDialog->setProperty(
                "message",
                QVariant::fromValue<QString>(errorString)
            );
            messageDialog->setTitle(YADAW::Base::ProductName);
            messageDialog->setModality(Qt::WindowModality::ApplicationModal);
            messageDialog->setVisible(true);
            YADAW::UI::focusMessageDialogButton(0);
            YADAW::UI::getMessageDialogResult();
        }
    }
    const auto& currentOutputDeviceId = backend.currentOutputDevice().id;
    int currentOutputDeviceIndex = -1;
    for(int i = 0; i < backend.audioOutputDeviceCount(); ++i)
    {
        if(backend.audioOutputDeviceAt(i).id == currentOutputDeviceId)
        {
            currentOutputDeviceIndex = i;
        }
    }
    QObject::connect(&YADAW::Controller::appAudioGraphInputDeviceListModel(),
        &QAbstractItemModel::dataChanged,
        &saveAudioBackendState);
    QObject::connect(&YADAW::Controller::appAudioGraphOutputDeviceListModel(),
        &QAbstractItemModel::dataChanged,
        &saveAudioBackendState);
    appConfig["audio-hardware"]["audiograph"] = audioGraphNode;
    YADAW::Controller::saveConfig(appConfig);
#elif __linux__
    auto& backend = YADAW::Controller::appALSABackend();
    // initialize backend
    QObject::connect(&YADAW::Controller::appALSAInputDeviceListModel(),
        &QAbstractItemModel::dataChanged, &saveAudioBackendState);
    QObject::connect(&YADAW::Controller::appALSAOutputDeviceListModel(),
        &QAbstractItemModel::dataChanged, &saveAudioBackendState);
    auto errors = YADAW::Controller::initializeALSAFromConfig(appConfig["audio-hardware"]["alsa"]);
#endif
    // -------------------------------------------------------------------------
    // initialize audio bus configuration --------------------------------------
    // -------------------------------------------------------------------------
    auto& audioBusConfiguration = YADAW::Controller::appAudioBusConfiguration();
    static YADAW::Model::AudioBusConfigurationModel appAudioBusInputConfigurationModel(
        audioBusConfiguration, true);
    static YADAW::Model::AudioBusConfigurationModel appAudioBusOutputConfigurationModel(
        audioBusConfiguration, false);
    if(auto audioBusConfigNode = appConfig["audio-bus"];
        audioBusConfigNode.IsDefined())
    {
        YADAW::Controller::loadAudioBusConfiguration(audioBusConfigNode,
            appAudioBusInputConfigurationModel, appAudioBusOutputConfigurationModel);
    }
    // -------------------------------------------------------------------------
    // initialize device graph and mixer----------------------------------------
    // -------------------------------------------------------------------------
    auto& engine = YADAW::Controller::AudioEngine::appAudioEngine();
    auto& mixer = engine.mixer();
    auto& appGraphWithPDC = mixer.graph();
    auto& appGraph = appGraphWithPDC.graph();
    auto& bufferExt = mixer.bufferExtension();
#if _WIN32
    engine.setSampleRate(backend.sampleRate());
    engine.setBufferSize(backend.bufferSizeInFrames());
#elif __linux__
    engine.setSampleRate(backend.sampleRate());
    engine.setBufferSize(backend.frameCount());
#endif
    // Add audio input channels
    FOR_RANGE0(i, audioBusConfiguration.inputBusCount())
    {
        auto& bus = audioBusConfiguration.getInputBusAt(i)->get();
        auto node = appGraphWithPDC.addNode(YADAW::Audio::Engine::AudioDeviceProcess(bus));
        mixer.appendAudioInputChannel(node, 0);
        auto& preFaderInserts = mixer.audioInputChannelPreFaderInsertsAt(i)->get();
        preFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
        preFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
        preFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
        auto& postFaderInserts = mixer.audioInputChannelPostFaderInsertsAt(i)->get();
        postFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
        postFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
        postFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
        auto& channelInfo = mixer.audioInputChannelInfoAt(i)->get();
        channelInfo.name = appAudioBusInputConfigurationModel.data(
            appAudioBusInputConfigurationModel.index(i),
            YADAW::Model::IAudioBusConfigurationModel::Role::Name
        ).value<QString>();
    }
    // Add audio output channels
    FOR_RANGE0(i, audioBusConfiguration.outputBusCount())
    {
        auto& bus = audioBusConfiguration.getOutputBusAt(i)->get();
        auto node = appGraphWithPDC.addNode(YADAW::Audio::Engine::AudioDeviceProcess(bus));
        mixer.appendAudioOutputChannel(node, 0);
        auto& preFaderInserts = mixer.audioOutputChannelPreFaderInsertsAt(i)->get();
        preFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
        preFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
        preFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
        auto& postFaderInserts = mixer.audioOutputChannelPostFaderInsertsAt(i)->get();
        postFaderInserts.setNodeAddedCallback(&YADAW::Controller::AudioEngine::insertsNodeAddedCallback);
        postFaderInserts.setNodeRemovedCallback(&YADAW::Controller::AudioEngine::insertsNodeRemovedCallback);
        postFaderInserts.setConnectionUpdatedCallback(&YADAW::Controller::AudioEngine::insertsConnectionUpdatedCallback);
        auto& channelInfo = mixer.audioOutputChannelInfoAt(i)->get();
        channelInfo.name = appAudioBusOutputConfigurationModel.data(
            appAudioBusOutputConfigurationModel.index(i),
            YADAW::Model::IAudioBusConfigurationModel::Role::Name
        ).value<QString>();
    }
    // Pass the process sequence into the audio callback
    auto& processSequence = engine.processSequence();
    processSequence.update(
        std::make_unique<YADAW::Audio::Engine::ProcessSequence>(
            YADAW::Audio::Engine::getProcessSequence(appGraph, bufferExt)
        )
    );
    // The following statement can be omitted since `AudioEngine::process`
    // already does that. But it's not a bad idea to do this in order to lessen
    // the work of the first callback.
    processSequence.swapIfNeeded();
    auto& vst3PluginPool = YADAW::Controller::appVST3PluginPool();
    YADAW::Audio::Host::CLAPHost::setMainThreadId(std::this_thread::get_id());
    // Start the audio backend
    engine.setRunning(true);
#if _WIN32
    backend.start(&YADAW::Controller::audioGraphCallback);
#elif __linux__
    backend.start(&YADAW::Controller::alsaCallback);
#endif
    // -------------------------------------------------------------------------
    // Open main window---------------------------------------------------------
    // -------------------------------------------------------------------------
    YADAW::UI::qmlApplicationEngine->loadFromModule("Main", "YADAW");
    YADAW::UI::mainWindow->setProperty("currentAudioBackend",
        QVariant::fromValue<int>(currentBackend));
    YADAW::UI::mainWindow->setProperty("assetDirectoryListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appAssetDirectoryListModel()));
    YADAW::UI::mainWindow->setProperty("pluginDirectoryListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appPluginDirectoryListModel()));
    auto& appPluginListModel = YADAW::Controller::appPluginListModel();
    YADAW::UI::mainWindow->setProperty("pluginListModel",
        QVariant::fromValue<QObject*>(&appPluginListModel));
    YADAW::UI::mainWindow->setProperty("systemFontRendering",
        QVariant::fromValue<bool>(YADAW::Controller::GeneralSettingsController::systemFontRendering()));
    YADAW::UI::mainWindow->setProperty("systemFontRenderingWhileDebugging",
        QVariant::fromValue<bool>(YADAW::Controller::GeneralSettingsController::systemFontRenderingWhileDebugging()));
    YADAW::UI::mainWindow->setProperty("translationModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appLocalizationListModel()));
    YADAW::UI::mainWindow->setProperty("currentTranslationIndex",
        QVariant::fromValue<int>(YADAW::Controller::currentTranslationIndex));
#if _WIN32
    YADAW::UI::mainWindow->setProperty("audioGraphInputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appAudioGraphInputDeviceListModel()));
    YADAW::UI::mainWindow->setProperty("audioGraphOutputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appAudioGraphOutputDeviceListModel()));
    YADAW::UI::mainWindow->setProperty("audioInputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appAudioGraphInputDeviceListModel()));
    YADAW::UI::mainWindow->setProperty("audioOutputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appAudioGraphOutputDeviceListModel()));
    if(currentOutputDeviceIndex != -1)
    {
        YADAW::UI::mainWindow->setProperty("audioGraphOutputDeviceIndex",
            QVariant::fromValue<int>(currentOutputDeviceIndex));
    }
#elif __linux__
    YADAW::UI::mainWindow->setProperty("alsaInputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appALSAInputDeviceListModel()));
    YADAW::UI::mainWindow->setProperty("alsaOutputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appALSAOutputDeviceListModel()));
    YADAW::UI::mainWindow->setProperty("audioInputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appALSAInputDeviceListModel()));
    YADAW::UI::mainWindow->setProperty("audioOutputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appALSAOutputDeviceListModel()));
#endif
    auto& audioInputMixerChannels = YADAW::Controller::appAudioInputMixerChannels();
    auto& audioOutputMixerChannels = YADAW::Controller::appAudioOutputMixerChannels();
    YADAW::UI::mainWindow->setProperty("mixerAudioInputChannelModel",
        QVariant::fromValue<QObject*>(&audioInputMixerChannels));
    YADAW::UI::mainWindow->setProperty("mixerAudioOutputChannelModel",
        QVariant::fromValue<QObject*>(&audioOutputMixerChannels));
    static YADAW::Model::HardwareAudioIOPositionModel hardwareAudioInputPositionModel(
        audioInputMixerChannels
    );
    static YADAW::Model::HardwareAudioIOPositionModel hardwareAudioOutputPositionModel(
        audioOutputMixerChannels
    );
    static YADAW::Model::RegularAudioIOPositionModel audioFXIOPositionModel(
        YADAW::Controller::appMixerChannels(),
        YADAW::Model::IMixerChannelListModel::ChannelTypes::ChannelTypeAudioFX
    );
    static YADAW::Model::RegularAudioIOPositionModel audioGroupIOPositionModel(
        YADAW::Controller::appMixerChannels(),
        YADAW::Model::IMixerChannelListModel::ChannelTypes::ChannelTypeBus
    );
    YADAW::UI::mainWindow->setProperty("audioHardwareInputPositionModel",
        QVariant::fromValue<QObject*>(&hardwareAudioInputPositionModel)
    );
    YADAW::UI::mainWindow->setProperty("audioHardwareOutputPositionModel",
        QVariant::fromValue<QObject*>(&hardwareAudioOutputPositionModel)
    );
    YADAW::UI::mainWindow->setProperty("audioGroupChannelModel",
        QVariant::fromValue<QObject*>(&audioGroupIOPositionModel)
    );
    YADAW::UI::mainWindow->setProperty("audioEffectChannelModel",
        QVariant::fromValue<QObject*>(&audioFXIOPositionModel)
    );
    QObject::connect(
        &appAudioBusInputConfigurationModel,
        &YADAW::Model::AudioBusConfigurationModel::dataChanged,
        [](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
        {
            if(roles.contains(YADAW::Model::AudioBusConfigurationModel::Role::Name))
            {
                auto& audioInputMixerChannels = YADAW::Controller::appAudioInputMixerChannels();
                FOR_RANGE(i, topLeft.row(), bottomRight.row() + 1)
                {
                    audioInputMixerChannels.setData(
                        audioInputMixerChannels.index(i),
                        appAudioBusInputConfigurationModel.data(
                            appAudioBusInputConfigurationModel.index(topLeft.row()),
                            YADAW::Model::IAudioBusConfigurationModel::Role::Name
                        ),
                        YADAW::Model::MixerChannelListModel::Role::Name
                    );
                }
            }
        }
    );
    QObject::connect(
        &appAudioBusOutputConfigurationModel,
        &YADAW::Model::AudioBusConfigurationModel::dataChanged,
        [](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
        {
            if(roles.contains(YADAW::Model::AudioBusConfigurationModel::Role::Name))
            {
                auto& audioOutputMixerChannels = YADAW::Controller::appAudioOutputMixerChannels();
                FOR_RANGE(i, topLeft.row(), bottomRight.row() + 1)
                {
                    audioOutputMixerChannels.setData(
                        audioOutputMixerChannels.index(i),
                        appAudioBusOutputConfigurationModel.data(
                            appAudioBusOutputConfigurationModel.index(topLeft.row()),
                            YADAW::Model::IAudioBusConfigurationModel::Role::Name
                        ),
                        YADAW::Model::MixerChannelListModel::Role::Name
                    );
                }
            }
        }
    );
    QObject::connect(
        &appAudioBusInputConfigurationModel,
        &YADAW::Model::AudioBusConfigurationModel::rowsInserted,
        [](const QModelIndex& parent, int first, int last)
        {
            auto& audioInputMixerChannels = YADAW::Controller::appAudioInputMixerChannels();
            audioInputMixerChannels.insert(
                first, 1,
                YADAW::Model::IMixerChannelListModel::ChannelTypes::ChannelTypeAudioHardwareInput,
                static_cast<YADAW::Entity::ChannelConfig::Config>(
                    appAudioBusInputConfigurationModel.data(
                        appAudioBusInputConfigurationModel.index(first),
                        YADAW::Model::IAudioBusConfigurationModel::Role::ChannelConfig
                    ).value<int>()
                )
            );
            audioInputMixerChannels.setData(
                audioInputMixerChannels.index(first),
                appAudioBusInputConfigurationModel.data(
                    appAudioBusInputConfigurationModel.index(first),
                    YADAW::Model::IAudioBusConfigurationModel::Role::Name
                ),
                YADAW::Model::MixerChannelListModel::Role::Name
            );
        }
    );
    QObject::connect(
        &appAudioBusOutputConfigurationModel,
        &YADAW::Model::AudioBusConfigurationModel::rowsInserted,
        [](const QModelIndex& parent, int first, int last)
        {
            auto& audioOutputMixerChannels = YADAW::Controller::appAudioOutputMixerChannels();
            audioOutputMixerChannels.insert(
                first, 1,
                YADAW::Model::IMixerChannelListModel::ChannelTypes::ChannelTypeAudioHardwareOutput,
                appAudioBusOutputConfigurationModel.data(
                    appAudioBusOutputConfigurationModel.index(first),
                    YADAW::Model::IAudioBusConfigurationModel::Role::ChannelConfig
                ).value<YADAW::Entity::ChannelConfig::Config>()
            );
            audioOutputMixerChannels.setData(
                audioOutputMixerChannels.index(first),
                appAudioBusOutputConfigurationModel.data(
                    appAudioBusOutputConfigurationModel.index(first),
                    YADAW::Model::IAudioBusConfigurationModel::Role::Name
                ),
                YADAW::Model::MixerChannelListModel::Role::Name
            );
        }
    );
    QObject::connect(
        &appAudioBusInputConfigurationModel,
        &YADAW::Model::AudioBusConfigurationModel::rowsAboutToBeRemoved,
        [](const QModelIndex& parent, int first, int last)
        {
            auto& audioInputMixerChannels = YADAW::Controller::appAudioInputMixerChannels();
            audioInputMixerChannels.remove(first, last - first + 1);
        }
    );
    QObject::connect(
        &appAudioBusOutputConfigurationModel,
        &YADAW::Model::AudioBusConfigurationModel::rowsAboutToBeRemoved,
        [](const QModelIndex& parent, int first, int last)
        {
            auto& audioOutputMixerChannels = YADAW::Controller::appAudioOutputMixerChannels();
            audioOutputMixerChannels.remove(first, last - first + 1);
        }
    );
    YADAW::UI::mainWindow->setProperty("mixerChannelModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appMixerChannels()));

    YADAW::UI::mainWindow->setProperty("audioInputBusConfigurationModel",
        QVariant::fromValue<QObject*>(&appAudioBusInputConfigurationModel));
    YADAW::UI::mainWindow->setProperty("audioOutputBusConfigurationModel",
        QVariant::fromValue<QObject*>(&appAudioBusOutputConfigurationModel));
    static auto saveAudioBusConfigurationLambda =
        []()
        {
            saveAudioBusConfiguration(
                YADAW::Controller::appAudioBusConfiguration(),
                appAudioBusInputConfigurationModel,
                appAudioBusOutputConfigurationModel);
        };
    QObject::connect(&appAudioBusInputConfigurationModel,
        &QAbstractItemModel::rowsInserted,
        saveAudioBusConfigurationLambda);
    QObject::connect(&appAudioBusOutputConfigurationModel,
        &QAbstractItemModel::rowsInserted,
        saveAudioBusConfigurationLambda);
    QObject::connect(&appAudioBusInputConfigurationModel,
        &QAbstractItemModel::rowsRemoved,
        saveAudioBusConfigurationLambda);
    QObject::connect(&appAudioBusOutputConfigurationModel,
        &QAbstractItemModel::rowsRemoved,
        saveAudioBusConfigurationLambda);
    QObject::connect(&appAudioBusInputConfigurationModel,
        &QAbstractItemModel::dataChanged,
        saveAudioBusConfigurationLambda);
    QObject::connect(&appAudioBusOutputConfigurationModel,
        &QAbstractItemModel::dataChanged,
        saveAudioBusConfigurationLambda);
    QObject::connect(YADAW::Event::eventSender, SIGNAL(currentAudioBackendChanged()),
        this, SLOT(onCurrentAudioBackendChanged()));
#if _WIN32
    QObject::connect(YADAW::Event::eventSender, SIGNAL(audioGraphOutputDeviceIndexChanged(int)),
        this, SLOT(onAudioGraphOutputDeviceIndexChanged(int)));
#endif
    QObject::connect(YADAW::Event::eventSender, SIGNAL(setSystemFontRendering(bool)),
        this, SLOT(onSetSystemFontRendering(bool)));
    QObject::connect(YADAW::Event::eventSender, SIGNAL(setSystemFontRenderingWhileDebugging(bool)),
        this, SLOT(onSetSystemFontRenderingWhileDebugging(bool)));
    QObject::connect(YADAW::Event::eventSender, SIGNAL(setTranslationIndex(int)),
        this, SLOT(onSetTranslationIndex(int)));
#if __linux__
    auto& eventFDSupport = YADAW::Audio::Host::EventFileDescriptorSupport::instance();
    eventFDSupport.start();
#endif
    auto version = QLibraryInfo::version();
    setQtVersion(version.majorVersion(), version.minorVersion(), version.microVersion());
    setQtCopyrightYear(QT_COPYRIGHT_YEAR);
    YADAW::Event::splashScreenWorkerThread->closeSplashScreen();
    auto& timer = YADAW::UI::idleProcessTimer();
    timer.callOnTimeout(
        this,
        []()
        {
            auto& engine = YADAW::Controller::AudioEngine::appAudioEngine();
            auto time = engine.processTime();
            YADAW::UI::mainWindow->setProperty(
                "cpuUsagePercentage",
                QVariant::fromValue<int>(time * engine.sampleRate() / (engine.bufferSize() * 1000000000LL) * 100)
            );
        }
    );
    YADAW::UI::mainWindow->setProperty(
        "nativePopupEventFilterModel",
        QVariant::fromValue<QObject*>(
            YADAW::Model::NativePopupEventFilterModel::create(*YADAW::UI::mainWindow)
        )
    );
    mainWindowReady();
}

void EventHandler::onMainWindowClosing()
{
#if __linux__
    auto& eventFDSupport = YADAW::Audio::Host::EventFileDescriptorSupport::instance();
    eventFDSupport.stop();
    eventFDSupport.clear();
#endif
    auto& timer = YADAW::UI::idleProcessTimer();
    QObject::disconnect(&timer, &QTimer::timeout, this, nullptr);
    timer.stop();
#if _WIN32
    YADAW::Controller::appAudioGraphBackend().uninitialize();
#elif __linux__
    YADAW::Controller::appALSABackend().uninitialize();
#endif
    auto& audioEngine = YADAW::Controller::AudioEngine::appAudioEngine();
    audioEngine.setRunning(false);
    YADAW::Controller::appAudioInputMixerChannels().clear();
    YADAW::Controller::appMixerChannels().clear();
    YADAW::Controller::appAudioOutputMixerChannels().clear();
    auto& graphWithPDC = audioEngine.mixer().graph();
    auto& graph = graphWithPDC.graph();
    audioEngine.uninitialize();
    mainWindowCloseAccepted();
}

void EventHandler::onLocateFileInExplorer(const QString& path)
{
    YADAW::Native::locateFileInExplorer(path);
}

void EventHandler::onStartPluginScan()
{
    std::thread([this]()
    {
        setScanningDirectories(true);
        YADAW::DAO::removeAllPluginCategories();
        YADAW::DAO::removeAllPlugins();
        YADAW::Controller::appPluginListModel().clear();
        const auto& model = YADAW::Controller::appPluginDirectoryListModel();
        auto itemCount = model.itemCount();
        std::vector<std::vector<QString>> libLists;
        int libCount = 0;
        FOR_RANGE0(i, itemCount)
        {
            QDir dir(model[i]);
            if(dir.exists())
            {
                auto& libList = libLists.emplace_back(
                    YADAW::Controller::scanDirectory(dir, true, true/*FIXME*/)
                );
                libCount += libList.size();
            }
        }
        setScanningDirectories(false);
        setPluginScanTotalCount(libCount);
        int currentIndex = 0;
        std::deque<YADAW::Controller::PluginScanResult> storeToDatabase;
        for(auto& libList: libLists)
        {
            for(auto& lib: libList)
            {
                auto name = QFileInfo(lib).completeBaseName();
                setPluginScanProgress(currentIndex++, name);
                lib = QDir::toNativeSeparators(lib);
                if(
                    std::none_of(
                        storeToDatabase.begin(), storeToDatabase.end(),
                        [&lib](const YADAW::Controller::PluginScanResult& rhs)
                        {
                            return rhs.pluginInfo.path == lib;
                        }
                    )
                )
                {
                    const auto& results = YADAW::Controller::scanSingleLibraryFile(lib);
                    for(const auto& result: results)
                    {
                        storeToDatabase.emplace_back(result);
                    }
                }
            }
        }
        for(const auto& result: storeToDatabase)
        {
            YADAW::Controller::savePluginScanResult(result);
        }
        storeToDatabase.clear();
        pluginScanComplete();
        YADAW::Controller::appPluginListModel().update();
    }).detach();
}

void EventHandler::onSetSystemFontRendering(bool enabled)
{
    YADAW::Controller::GeneralSettingsController::setSystemFontRendering(enabled);
}

void EventHandler::onSetSystemFontRenderingWhileDebugging(bool enabled)
{
    YADAW::Controller::GeneralSettingsController::setSystemFontRenderingWhileDebugging(enabled);
}

void EventHandler::onSetTranslationIndex(int index)
{
    const auto& model = YADAW::Controller::appLocalizationListModel();
    if(index >= 0 && index < model.itemCount() && index != YADAW::Controller::currentTranslationIndex)
    {
        YADAW::Controller::currentTranslationIndex = index;
        YADAW::Controller::GeneralSettingsController::setTranslation(model.at(index).name);
    }
    else
    {
        YADAW::UI::mainWindow->setProperty("currentTranslationIndex",
            QVariant::fromValue<int>(YADAW::Controller::currentTranslationIndex));
    }
}

void EventHandler::onCurrentAudioBackendChanged()
{
    auto backend = static_cast<YADAW::Entity::AudioBackendSupport::Backend>(YADAW::UI::mainWindow->property("currentAudioBackend").value<int>());
    auto config = YADAW::Controller::loadConfig();
    auto audioHardwareNode = config["audio-hardware"];
    YADAW::Controller::saveBackendToConfig(backend, audioHardwareNode);
    YADAW::Controller::saveConfig(config);
}

void EventHandler::onAudioGraphOutputDeviceIndexChanged(int index)
{
#if _WIN32
    YADAW::Controller::appAudioGraphOutputDeviceListModel().setOutputDeviceIndex(index);
#endif
}

void EventHandler::onSetMainWindowFromMaximizedToFullScreen()
{
    YADAW::UI::setMaximizedWindowToFullScreen(*YADAW::UI::mainWindow);
}

void EventHandler::onSetMainWindowFromFullScreenToMaximized()
{
    YADAW::UI::setFullScreenWindowToMaximized(*YADAW::UI::mainWindow);
}
}
