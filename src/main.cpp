#include "controller/ConfigController.hpp"
#include "controller/LocalizationController.hpp"
#include "event/EventBase.hpp"
#include "event/EventHandler.hpp"
#include "event/SplashScreenWorkerThread.hpp"
#include "model/ModelInitializer.hpp"
#include "native/WindowsDarkModeSupport.hpp"
#include "ui/UI.hpp"

#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

int main(int argc, char *argv[])
{
    qputenv("QSG_RENDER_LOOP", "basic");
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    YADAW::UI::qmlApplicationEngine = &engine;
    QFont::insertSubstitution("Fira Sans", "WenQuanYi Micro Hei");
    YADAW::Model::initializeModel();
    const QUrl frontendEventsURL(u"qrc:Main/Events.qml"_qs);
    const QUrl splashScreenURL(u"qrc:content/SplashScreen.qml"_qs);
    QObject* splashScreen = nullptr;
    const QUrl url(u"qrc:Main/YADAW.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [&](QObject *obj, const QUrl &objUrl) {
        if (!obj)
        {
#if(NDEBUG)
#else
            fprintf(stderr, "Press <ENTER> to exit...");
            getchar();
#endif
            QCoreApplication::exit(-1);
        }
        if(objUrl == frontendEventsURL)
        {
            YADAW::Event::eventSender = obj->property("eventSender").value<QObject*>();
            YADAW::Event::eventReceiver = obj->property("eventReceiver").value<QObject*>();
        }
        else if(objUrl == splashScreenURL)
        {
            splashScreen = obj;
        }
        else if(objUrl == url)
        {
            YADAW::UI::mainWindow = qobject_cast<QQuickWindow*>(obj);
            YADAW::Native::WindowsDarkModeSupport::instance()->addWindow(qobject_cast<QWindow*>(obj));
        }
    }, Qt::DirectConnection);
    YADAW::Controller::initializeApplicationConfig();
    auto config = YADAW::Controller::loadConfig();
    auto language = QString::fromStdString(config["general"]["language"].as<std::string>());
    auto& localizationList = YADAW::Controller::appLocalizationListModel();
    QTranslator translator;
    for(int i = 0; i < localizationList.itemCount(); ++i)
    {
        const auto& localization = localizationList.at(i);
        if(localization.name == language && localization.translationFileName != "")
        {
            if(translator.load(localization.translationFileName))
            {
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
                YADAW::Controller::currentTranslationIndex = i;
            }
            else
            {
                // TODO: Inform the user that translation file cannot be loaded
            }
            break;
        }
    }
    if(YADAW::Controller::currentTranslationIndex == -1)
    {
        YADAW::Controller::currentTranslationIndex = 0;
    }
    auto systemFontRendering = config["general"]["system-font-rendering"].as<bool>();
    if(systemFontRendering)
    {
        QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    }
    engine.load(frontendEventsURL);
    YADAW::Event::EventHandler eh(YADAW::Event::eventSender, YADAW::Event::eventReceiver);
    YADAW::Event::eventHandler = &eh;
    engine.load(splashScreenURL);
    YADAW::Event::SplashScreenWorkerThread sswt(splashScreen);
    YADAW::Event::splashScreenWorkerThread = &sswt;
    QObject::connect(&sswt, &YADAW::Event::SplashScreenWorkerThread::openMainWindow,
        &eh, &YADAW::Event::EventHandler::onOpenMainWindow,
        Qt::ConnectionType::QueuedConnection);
    sswt.start();
    // engine.load(url);
    // eventHandler.setQtVersion(QString(qVersion()));
    auto ret = app.exec();
    YADAW::Native::WindowsDarkModeSupport::instance().reset();
    return ret;
}
