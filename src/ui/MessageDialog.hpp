#ifndef YADAW_SRC_UI_MESSAGEDIALOG
#define YADAW_SRC_UI_MESSAGEDIALOG

#include <QString>

class QQuickWindow;

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

void focusMessageDialogButton(int index);

int getMessageDialogResult();
}

#endif // YADAW_SRC_UI_MESSAGEDIALOG
