#include "EventHandler.hpp"

#include "base/Constants.hpp"
#include "controller/AppController.hpp"
#include "controller/AssetDirectoryController.hpp"
#if(WIN32)
#include "controller/AudioGraphBackendController.hpp"
#elif(__linux__)
#include "controller/ALSABackendController.hpp"
#endif
#include "controller/AudioBusConfigurationController.hpp"
#include "controller/ConfigController.hpp"
#include "controller/GeneralSettingsController.hpp"
#include "controller/PluginController.hpp"
#include "controller/PluginDirectoryController.hpp"
#include "controller/PluginListController.hpp"
#include "controller/LocalizationController.hpp"
#include "event/EventBase.hpp"
#include "native/Native.hpp"
#include "ui/MessageDialog.hpp"
#include "ui/UI.hpp"

#include <QDir>

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
#if(WIN32)
    const auto& audioGraphConfig = YADAW::Controller::deviceConfigFromCurrentAudioGraph();
    appConfig["audio-hardware"]["audiograph"] = audioGraphConfig;
    auto dump = YAML::Dump(audioGraphConfig);
    YADAW::Controller::saveConfig(appConfig);
#elif(__linux__)
    const auto& alsaConfig = YADAW::Controller::deviceConfigFromALSA();
    appConfig["audio-hardware"]["alsa"] = alsaConfig;
    auto dump = YAML::Dump(alsaConfig);
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
}

void EventHandler::connectToEventReceiver(QObject* receiver)
{
    QObject::connect(this, SIGNAL(mainWindowReady()),
        receiver, SIGNAL(mainWindowReady()));
    QObject::connect(this, SIGNAL(mainWindowCloseAccepted()),
        receiver, SIGNAL(mainWindowClosingAccepted()));
    QObject::connect(this, SIGNAL(setQtVersion(QString)),
        receiver, SIGNAL(setQtVersion(QString)));
    QObject::connect(this, SIGNAL(setSplashScreenText(QString)),
        receiver, SIGNAL(setSplashScreenText(QString)));
    QObject::connect(this, SIGNAL(pluginScanComplete()),
        receiver, SIGNAL(pluginScanComplete()));
    QObject::connect(this, SIGNAL(messageDialog(QString, QString, int, bool)),
        receiver, SIGNAL(messageDialog(QString, QString, int, bool)));
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
    // initialize audio backend
#if(WIN32)
    auto& backend = YADAW::Controller::appAudioGraphBackend();
    backend.initialize();
    backend.createAudioGraph();
    auto audioGraphNode = appConfig["audio-hardware"]["audiograph"];
    if(audioGraphNode.IsNull())
    {
        YADAW::Controller::activateDefaultDevice(backend);
        appConfig["audio-hardware"]["audiograph"] = YADAW::Controller::deviceConfigFromCurrentAudioGraph();
        YADAW::Controller::saveConfig(appConfig);
    }
    else
    {
        YADAW::Controller::createAudioGraphFromConfig(audioGraphNode);
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
#elif(__linux__)
    auto& backend = YADAW::Controller::appALSABackend();
    // initialize backend
    QObject::connect(&YADAW::Controller::appALSAInputDeviceListModel(),
        &QAbstractItemModel::dataChanged, &saveAudioBackendState);
    QObject::connect(&YADAW::Controller::appALSAOutputDeviceListModel(),
        &QAbstractItemModel::dataChanged, &saveAudioBackendState);
    YADAW::Controller::initializeALSAFromConfig(appConfig["audio-hardware"]["alsa"]);
#endif
    static YADAW::Model::AudioBusConfigurationModel appAudioBusInputConfigurationModel(
        YADAW::Controller::appAudioBusConfiguration(), true);
    static YADAW::Model::AudioBusConfigurationModel appAudioBusOutputConfigurationModel(
        YADAW::Controller::appAudioBusConfiguration(), false);
    if(auto audioBusConfigNode = appConfig["audio-bus"];
        audioBusConfigNode.IsDefined())
    {
        YADAW::Controller::loadAudioBusConfiguration(audioBusConfigNode,
            appAudioBusInputConfigurationModel, appAudioBusOutputConfigurationModel);
    }
    const QUrl mainWindowUrl(u"qrc:Main/YADAW.qml"_qs);
    YADAW::UI::qmlApplicationEngine->load(mainWindowUrl);
    YADAW::UI::mainWindow->setProperty("assetDirectoryListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appAssetDirectoryListModel()));
    YADAW::UI::mainWindow->setProperty("pluginDirectoryListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appPluginDirectoryListModel()));
    YADAW::UI::mainWindow->setProperty("pluginListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appPluginListModel()));
    YADAW::UI::mainWindow->setProperty("midiEffectListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appMIDIEffectListModel()));
    YADAW::UI::mainWindow->setProperty("instrumentListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appInstrumentListModel()));
    YADAW::UI::mainWindow->setProperty("audioEffectListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appAudioEffectListModel()));
    YADAW::UI::mainWindow->setProperty("vestifalPluginListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appVestifalPluginListModel()));
    YADAW::UI::mainWindow->setProperty("vst3PluginListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appVST3PluginListModel()));
    YADAW::UI::mainWindow->setProperty("clapPluginListModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appCLAPPluginListModel()));
    YADAW::UI::mainWindow->setProperty("systemFontRendering",
        QVariant::fromValue<bool>(YADAW::Controller::GeneralSettingsController::systemFontRendering()));
    YADAW::UI::mainWindow->setProperty("systemFontRenderingWhileDebugging",
        QVariant::fromValue<bool>(YADAW::Controller::GeneralSettingsController::systemFontRenderingWhileDebugging()));
    YADAW::UI::mainWindow->setProperty("translationModel",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appLocalizationListModel()));
    YADAW::UI::mainWindow->setProperty("currentTranslationIndex",
        QVariant::fromValue<int>(YADAW::Controller::currentTranslationIndex));
#if(WIN32)
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
#elif(__linux__)
    YADAW::UI::mainWindow->setProperty("alsaInputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appALSAInputDeviceListModel()));
    YADAW::UI::mainWindow->setProperty("alsaOutputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appALSAOutputDeviceListModel()));
    YADAW::UI::mainWindow->setProperty("audioInputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appALSAInputDeviceListModel()));
    YADAW::UI::mainWindow->setProperty("audioOutputDeviceList",
        QVariant::fromValue<QObject*>(&YADAW::Controller::appALSAOutputDeviceListModel()));
#endif
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
    QObject::connect(YADAW::Event::eventSender, SIGNAL(audioGraphOutputDeviceIndexChanged(int)),
        this, SLOT(onAudioGraphOutputDeviceIndexChanged(int)));
    QObject::connect(YADAW::Event::eventSender, SIGNAL(setSystemFontRendering(bool)),
        this, SLOT(onSetSystemFontRendering(bool)));
    QObject::connect(YADAW::Event::eventSender, SIGNAL(setSystemFontRenderingWhileDebugging(bool)),
        this, SLOT(onSetSystemFontRenderingWhileDebugging(bool)));
    QObject::connect(YADAW::Event::eventSender, SIGNAL(setTranslationIndex(int)),
        this, SLOT(onSetTranslationIndex(int)));
    setQtVersion(qVersion());
    YADAW::Event::splashScreenWorkerThread->closeSplashScreen();
    mainWindowReady();
}

void EventHandler::onMainWindowClosing()
{
#if(WIN32)
    YADAW::Controller::appAudioGraphBackend().uninitialize();
#elif(__linux__)
    YADAW::Controller::appALSABackend().uninitialize();
#endif
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
        YADAW::DAO::removeAllPluginCategories();
        YADAW::DAO::removeAllPlugins();
        YADAW::Controller::appPluginListModel().clear();
        YADAW::Controller::appMIDIEffectListModel().clear();
        YADAW::Controller::appInstrumentListModel().clear();
        YADAW::Controller::appAudioEffectListModel().clear();
        const auto& model = YADAW::Controller::appPluginDirectoryListModel();
        auto itemCount = model.itemCount();
        std::vector<std::vector<QString>> libLists;
        for(decltype(itemCount) i = 0; i < itemCount; ++i)
        {
            QDir dir(model[i]);
            if(dir.exists())
            {
                libLists.emplace_back(YADAW::Controller::scanDirectory(dir, true, true/*FIXME*/));
            }
        }
        std::deque<YADAW::Controller::PluginScanResult> storeToDatabase;
        for(auto& libList: libLists)
        {
            for(auto& lib: libList)
            {
                lib = QDir::toNativeSeparators(lib);
                if(std::none_of(storeToDatabase.begin(), storeToDatabase.end(),
                    [&lib](const YADAW::Controller::PluginScanResult& rhs)
                    {
                        return rhs.pluginInfo.path == lib;
                    }))
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
        YADAW::Controller::appMIDIEffectListModel().update();
        YADAW::Controller::appInstrumentListModel().update();
        YADAW::Controller::appAudioEffectListModel().update();
        YADAW::Controller::appVestifalPluginListModel().update();
        YADAW::Controller::appVST3PluginListModel().update();
        YADAW::Controller::appCLAPPluginListModel().update();
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

void EventHandler::onAudioGraphOutputDeviceIndexChanged(int index)
{
#if(WIN32)
    YADAW::Controller::appAudioGraphOutputDeviceListModel().setOutputDeviceIndex(index);
#endif
}
}
