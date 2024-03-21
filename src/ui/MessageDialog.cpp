#include "MessageDialog.hpp"

#include "event/EventBase.hpp"
#include "ui/UI.hpp"

namespace YADAW::UI
{
QQuickWindow* messageDialog = nullptr;

void createMessageDialog()
{
    qmlApplicationEngine->loadFromModule("content", "MessageDialog");
}
}