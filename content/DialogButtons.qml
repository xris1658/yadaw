import QtQuick

import YADAW.Models

Row {
    id: root
    property alias model: buttons.model
    signal focusButton(buttonIndex: int)
    onFocusButton: (buttonIndex) => {
        let button = buttons.itemAt(buttonIndex);
        if(button) {
            button.forceActiveFocus();
        }
    }
    QtObject {
        id: impl
        readonly property var buttonText: [
            Constants.okTextWithMnemonic,
            Constants.cancelTextWithMnemonic,
            Constants.applyTextWithMnemonic,
            Constants.yesTextWithMnemonic,
            Constants.noTextWithMnemonic,
            Constants.yesToAllTextWithMnemonic,
            Constants.noToAllTextWithMnemonic,
            Constants.abortTextWithMnemonic,
            Constants.retryTextWithMnemonic,
            Constants.ignoreTextWithMnemonic
        ]
    }
    spacing: 3
    Repeater {
        id: buttons
        model: ListModel {
            ListElement {
                buttonRole: MessageDialogButtonBoxModel.ButtonRoleOk
            }
        }
        Button {
            text: model.mdbbm_button_text != 0? mdbbm_button_text: impl.buttonText[mdbbm_button_role - MessageDialogButtonBoxModel.ButtonRoleOk]
            onClicked: {
                root.buttonClicked(index);
            }
        }
    }
    signal buttonClicked(buttonIndex: int)
}