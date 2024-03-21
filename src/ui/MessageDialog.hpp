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

extern QQuickWindow* messageDialog;

void createMessageDialog();
}

#endif // YADAW_SRC_UI_MESSAGEDIALOG
