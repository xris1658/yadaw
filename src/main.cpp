#include "base/Constants.hpp"
#include "controller/ConfigController.hpp"
#include "controller/LocalizationController.hpp"
#include "controller/PluginWindowController.hpp"
#include "event/EventBase.hpp"
#include "event/EventHandler.hpp"
#include "event/SplashScreenWorkerThread.hpp"
#include "entity/EntityInitializer.hpp"
#include "model/ModelInitializer.hpp"
#include "native/Native.hpp"
#include "ui/MessageDialog.hpp"
#include "ui/UI.hpp"

#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>

int main(int argc, char *argv[])
{
    qputenv("QSG_NO_VSYNC", "1");
    YADAW::Native::fillCPUIDInfo();
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    YADAW::UI::qmlApplicationEngine = &engine;
    YADAW::Entity::initializeEntity();
    YADAW::Model::initializeModel();
    const QString eventsName("Events.qml");
    const QString splashScreenName("SplashScreen.qml");
    QObject* splashScreen = nullptr;
    const QString mainWindowName("YADAW.qml");
    const QString pluginWindowName("PluginWindow.qml");
    const QString genericPluginEditorName("GenericPluginEditor.qml");
    const QString messageDialogName("MessageDialog.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app,
        [&](QObject *obj, const QUrl &objUrl)
        {
            if(!obj)
            {
#if NDEBUG
#else
                fprintf(stderr, "Press <ENTER> to exit...");
                getchar();
#endif
                std::exit(-1);
            }
            const auto& fileName = objUrl.fileName();
            if(fileName == eventsName)
            {
                YADAW::Event::eventSender = obj->property("eventSender").value<QObject*>();
                YADAW::Event::eventReceiver = obj->property("eventReceiver").value<QObject*>();
            }
            else if(fileName == splashScreenName)
            {
                splashScreen = obj;
            }
            else if(fileName == mainWindowName)
            {
                YADAW::UI::mainWindow = qobject_cast<QQuickWindow*>(obj);
            }
            else if(fileName == pluginWindowName)
            {
                YADAW::Controller::pluginWindows.pluginWindow = qobject_cast<QWindow*>(obj);
            }
            else if(fileName == genericPluginEditorName)
            {
                YADAW::Controller::pluginWindows.genericEditorWindow = qobject_cast<QWindow*>(obj);
            }
            else if(fileName == messageDialogName)
            {
                YADAW::UI::messageDialog = qobject_cast<QQuickWindow*>(obj);
            }
        },
        Qt::DirectConnection
    );
    YADAW::Controller::initializeApplicationConfig();
    auto config = YADAW::Controller::loadConfig();
    auto systemFontRendering = config["general"]["system-font-rendering"].as<bool>();
    if(systemFontRendering)
    {
        if((!YADAW::Native::isDebuggerPresent()) || config["general"]["system-font-rendering-while-debugging"].as<bool>())
        {
            QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
        }
    }
    QDir dir(YADAW::UI::defaultFontDir());
    if(dir.exists())
    {
        const auto& entryInfoList = dir.entryInfoList(QDir::Filter::Files | QDir::Filter::Hidden);
        for(const auto& entryInfo: entryInfoList)
        {
            QFontDatabase::addApplicationFont(entryInfo.absoluteFilePath());
        }
    }
    auto language = QString::fromStdString(config["general"]["language"].as<std::string>());
    auto& localizationList = YADAW::Controller::appLocalizationListModel();
    QTranslator translator;
    for(int i = 0; i < localizationList.itemCount(); ++i)
    {
        const auto& localization = localizationList.at(i);
        if(localization.name == language && (!localization.translationFileList.empty()))
        {
            const auto& translationFileList = localization.translationFileList;
            decltype(translationFileList.size()) fileCountLoaded = std::count_if(
                translationFileList.begin(), translationFileList.end(),
                [&localization, &translator](const QString& translationFile)
                {
                    return translator.load(QLocale(localization.languageCode), translationFile);
                }
            );
            QCoreApplication::installTranslator(&translator);
            for(const auto& font: localization.fontList)
            {
                QFontDatabase::addApplicationFont(font);
            }
            QString fontName("Fira Sans");
            for(const auto& fontFamily: localization.fontFamilyList)
            {
                QFont::insertSubstitution(fontName, fontFamily);
            }
            if(fileCountLoaded != translationFileList.size())
            {
                YADAW::UI::createMessageDialog();
                auto messageDialog = YADAW::UI::messageDialog;
                if(messageDialog)
                {
                    messageDialog->setProperty(
                        "icon",
                        QVariant::fromValue<int>(YADAW::UI::IconType::Warning)
                    );
                    messageDialog->setProperty(
                        "message",
                        QVariant::fromValue<QString>(
                            "Failed to load some translation files. YADAW will use the files loaded, with English as fallback."
                        )
                    );
                    messageDialog->setTitle(YADAW::Base::ProductName);
                    messageDialog->setModality(Qt::WindowModality::ApplicationModal);
                    messageDialog->setVisible(true);
                }
                messageDialog->showNormal();
                YADAW::UI::getMessageDialogResult();
            }
            YADAW::Controller::currentTranslationIndex = i;
            break;
        }
    }
    if(YADAW::Controller::currentTranslationIndex == -1)
    {
        YADAW::UI::createMessageDialog();
        auto dialog = YADAW::UI::messageDialog;
        auto messageDialog = YADAW::UI::messageDialog;
        if(messageDialog)
        {
            messageDialog->setProperty(
                "icon",
                QVariant::fromValue<int>(YADAW::UI::IconType::Warning)
            );
            messageDialog->setProperty(
                "message",
                QVariant::fromValue<QString>("The selecting translation file does not exist. YADAW is showing English as fallback.")
            );
            messageDialog->setTitle(YADAW::Base::ProductName);
            messageDialog->setModality(Qt::WindowModality::ApplicationModal);
            messageDialog->setVisible(true);
        }
        messageDialog->showNormal();
        YADAW::UI::getMessageDialogResult();
        YADAW::Controller::currentTranslationIndex = 0;
    }
    engine.loadFromModule("Main", "Events");
    YADAW::Event::EventHandler eh(YADAW::Event::eventSender, YADAW::Event::eventReceiver);
    YADAW::Event::eventHandler = &eh;
    engine.loadFromModule("content", "SplashScreen");
    YADAW::Event::SplashScreenWorkerThread sswt(splashScreen);
    YADAW::Event::splashScreenWorkerThread = &sswt;
    QObject::connect(&sswt, &YADAW::Event::SplashScreenWorkerThread::openMainWindow,
        &eh, &YADAW::Event::EventHandler::onOpenMainWindow,
        Qt::ConnectionType::QueuedConnection);
    sswt.start();
    // engine.load(url);
    // eventHandler.setQtVersion(QString(qVersion()));
    auto ret = app.exec();
    return ret;
}
