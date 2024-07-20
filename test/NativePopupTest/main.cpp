#include "EventHandler.hpp"

#include "ui/NativePopupEventFilter.hpp"
#include "util/IntegerRange.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QVariant>
#include <QWindow>


int main(int argc, char** argv)
{
    QGuiApplication application(argc, argv);
    QQmlApplicationEngine engine;
    QWindow* mainWindow = nullptr;
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        [&](QObject* object, const QUrl& url)
        {
            mainWindow = static_cast<QWindow*>(object);
        }
    );
    engine.loadFromModule("NativePopupTestContent", "MainWindow");
    YADAW::UI::NativePopupEventFilter eventFilter(*mainWindow);
    auto nativePopup = static_cast<QWindow*>(mainWindow->property("nativePopup").value<QObject*>());
    QObject::connect(
        nativePopup, &QWindow::visibleChanged,
        [&](bool visible)
        {
            if(visible)
            {
                nativePopup->raise();
                QObject::connect(
                    &eventFilter, SIGNAL(mousePressedOutside()),
                    nativePopup, SIGNAL(mousePressedOutside())
                );
                eventFilter.append(*nativePopup);
            }
            else
            {
                FOR_RANGE0(i, eventFilter.count())
                {
                    if(&eventFilter[i] == nativePopup)
                    {
                        QObject::disconnect(
                            &eventFilter, &YADAW::UI::NativePopupEventFilter::mousePressedOutside,
                            nativePopup, nullptr
                        );
                        eventFilter.remove(i, 1);
                    }
                }
            }
        }
    );
    EventHandler eventHandler(*mainWindow);
    return application.exec();
}
