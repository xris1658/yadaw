#include "MessageDialog.hpp"

#include "event/EventBase.hpp"
#include "ui/UI.hpp"

#include <QEventLoop>

namespace YADAW::UI
{
QQuickWindow* messageDialog = nullptr;

void createMessageDialog()
{
    qmlApplicationEngine->loadFromModule("content", "MessageDialog");
}

int getMessageDialogResult()
{
    QEventLoop loop;
    QObject::connect(
        messageDialog, SIGNAL(clicked(int)),
        &loop, SLOT(exit(int))
    );
    return loop.exec();
}
}
