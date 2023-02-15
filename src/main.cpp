#include "event/EventBase.hpp"
#include "event/EventHandler.hpp"
#include "native/WindowsDarkModeSupport.hpp"
#include "ui/UI.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    YADAW::UI::qmlApplicationEngine = &engine;
    const QUrl splashScreenURL(u"qrc:content/Splashscreen.qml"_qs);
    const QUrl url(u"qrc:Main/YADAW.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj)
        {
#if(NDEBUG)
#else
            fprintf(stderr, "Press <ENTER> to exit...");
            getchar();
#endif
            QCoreApplication::exit(-1);
        }
        YADAW::Event::eventSender = obj->property("eventSender").value<QObject*>();
        YADAW::Event::eventReceiver = obj->property("eventReceiver").value<QObject*>();
        YADAW::Native::WindowsDarkModeSupport::instance()->addWindow(qobject_cast<QWindow*>(obj));
    }, Qt::DirectConnection);
    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    // engine.load(splashScreenURL);
    engine.load(url);
    YADAW::Event::EventHandler eventHandler(YADAW::Event::eventSender, YADAW::Event::eventReceiver);
    eventHandler.setQtVersion(QString(qVersion()));
    auto ret = app.exec();
    return ret;
}
