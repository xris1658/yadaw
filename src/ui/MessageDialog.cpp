#include "MessageDialog.hpp"

#include "event/EventBase.hpp"
#include "ui/Runtime.hpp"

#include <QEventLoop>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QVariant>

namespace YADAW::UI
{
QQuickWindow* messageDialog = nullptr;

void createMessageDialog()
{
    qmlApplicationEngine->loadFromModule("content", "MessageDialog");
}

void setHideCloseButton(bool hide)
{
    if(messageDialog)
    {
        messageDialog->setProperty(
            "removeCloseButton",
            QVariant::fromValue<bool>(hide)
        );
    }
}

void focusMessageDialogButton(int index)
{
    if(messageDialog)
    {
        const auto* metaObject = messageDialog->metaObject();
        auto signalIndex = metaObject->indexOfSignal("focusButton(int)");
        auto metaMethod = metaObject->method(signalIndex);
        metaMethod.invoke(messageDialog, index);
    }
}

int getMessageDialogResult()
{
    if(messageDialog)
    {
        QEventLoop loop;
        QObject::connect(
            messageDialog, SIGNAL(clicked(int)),
            &loop, SLOT(exit(int))
        );
        return loop.exec();
    }
    return -1;
}
}
