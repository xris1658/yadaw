#include "MessageDialog.hpp"

#include "event/EventBase.hpp"

namespace YADAW::UI
{
void messageDialog(const QString& message, const QString& title, IconType icon, bool modal)
{
    YADAW::Event::eventHandler->messageDialog(message, title, icon, modal);
}
}