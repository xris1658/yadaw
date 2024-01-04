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
            Constants.okText,
            Constants.cancelText,
            Constants.applyText,
            Constants.yesText,
            Constants.noText,
            Constants.yesToAllText,
            Constants.noToAllText,
            Constants.abortText,
            Constants.retryText,
            Constants.ignoreText
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