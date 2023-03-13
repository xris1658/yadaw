#ifndef YADAW_SRC_UI_MESSAGEDIALOG
#define YADAW_SRC_UI_MESSAGEDIALOG

#include <QString>

namespace YADAW::UI
{
enum IconType
{
    None,
    Question,
    Warning,
    Error,
    Check,
    Info
};

void messageDialog(const QString& message, const QString& title, IconType icon = IconType::None, bool modal = true);
}

#endif //YADAW_SRC_UI_MESSAGEDIALOG
