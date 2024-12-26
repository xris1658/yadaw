#include "audio/plugin/PluginText.hpp"
#include "base/Constants.hpp"
#include "controller/ConfigController.hpp"
#include "controller/LocalizationController.hpp"
#include "controller/PluginWindowController.hpp"
#include "event/EventBase.hpp"
#include "event/EventHandler.hpp"
#include "event/SplashScreenWorkerThread.hpp"
#include "entity/EntityInitializer.hpp"
#include "model/ModelInitializer.hpp"
#include "native/CPU.hpp"
#include "native/Native.hpp"
#include "ui/MessageDialog.hpp"
#include "ui/Runtime.hpp"
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
// In Qt 6.8, menu bars are shown as native ones by default, which is in good
// intent. But since it's limited by macOS, we do NOT use that feature on
// macOS for now.
// On macOS, contents in App menu (the one between Apple system menu and File
// menu) are fixed as "About", "Preferences", "Services", "Show/Hide" and
// "Quit". Seems like there is no way in Qt to add your own menu items other
// than those listed above.
// If there are multiple menu items that begin with "about" (case-insensitive)
// in any menu, then:
// - Only the last item will be present in the App menu, with
//   its text fixed as the localized string of "About <app name>". The text is
//   affected only by your system language and localized app name. Seems like Qt
//   determines the menu item shown as "About" by pattern-matching all of them.
// - The other items that begin with "about" disappeared. There's no way to find
//   these vanished items, even searching for it in "Help" results in nothing.
// This behavior allows some very surprising things to happen:
// - If I have two menu items named "About YADAW..." and "About Qt..." (see
//   MainWindow.qml), then "About Qt..." is shown in App menu with its text
//   changed to localized "About YADAW". Could you imagine triggering the menu
//   item called "About YADAW", only to find out an "About Qt" window appeared
//   for no reason?
// What's worse, you find this weird behavior ONLY by testing it on macOS.
//
// There are some applications that adds other menu items in the App menu, like
// "Secure keyboard entry" of Terminal, which shows below "Preferences". For now
// I'm not sure how it is implemented.
#if (QT_VERSION >= QT_VERSION_CHECK(6, 8, 0) && __APPLE__)
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
#endif
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
        if(localization.name == language)
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
                        QVariant::fromValue<QString>(
                            "Failed to load some translation files. YADAW will use the files loaded, with English as fallback."
                        )
                    );
                    messageDialog->setTitle(YADAW::Base::ProductName);
                    messageDialog->setModality(Qt::WindowModality::ApplicationModal);
                }
                YADAW::UI::focusMessageDialogButton(0);
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
                QVariant::fromValue<QString>("The selecting translation file does not exist. YADAW is showing English as fallback.")
            );
            messageDialog->setTitle(YADAW::Base::ProductName);
            messageDialog->setModality(Qt::WindowModality::ApplicationModal);
            messageDialog->setVisible(true);
            qDebug("focusMessageDialogButton");
            YADAW::UI::focusMessageDialogButton(0);
            messageDialog->showNormal();
            YADAW::UI::getMessageDialogResult();
        }
        YADAW::Controller::currentTranslationIndex = 0;
    }
    if(YADAW::Native::isDebuggerPresent())
    {
        YADAW::UI::createMessageDialog();
        YADAW::UI::setHideCloseButton(true);
        if(auto messageDialog = YADAW::UI::messageDialog)
        {
            messageDialog->setProperty(
                "icon",
                QVariant::fromValue<int>(YADAW::UI::IconType::Warning)
            );
            messageDialog->setProperty(
                "message",
                QVariant::fromValue<QString>(
                    YADAW::Audio::Plugin::getDebugModeWarningText()
                )
            );
            messageDialog->setTitle(
                YADAW::Audio::Plugin::getDebugModeWarningTitle()
            );
            messageDialog->setModality(Qt::WindowModality::ApplicationModal);
            messageDialog->setVisible(true);
            qDebug("focusMessageDialogButton");
            YADAW::UI::focusMessageDialogButton(0);
            messageDialog->showNormal();
            YADAW::UI::getMessageDialogResult();
        }
    }
    auto& timer = YADAW::UI::idleProcessTimer();
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
