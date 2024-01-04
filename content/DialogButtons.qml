import QtQuick

Row {
    id: root
    property alias model: buttons.model
    enum ButtonRole {
        Ok, Cancel, Apply,
        Yes, No,
        YesToAll, NoToAll,
        Abort, Retry, Ignore
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
                buttonRole: DialogButtons.Ok
            }
        }
        Button {
            property int role: buttonRole
            text: model.buttonText !== undefined? buttonText: impl.buttonText[role - DialogButtons.Ok]
            onClicked: {
                root.buttonClicked(role);
            }
        }
    }
    signal buttonClicked(role: int)
}