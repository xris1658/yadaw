import QtQuick
import QtQuick.Window

import YADAW.Models

Window {
    id: root
    // ui/MessageDialog.hpp
    enum Icon {
        None,
        Question,
        Warning,
        Error,
        Check,
        Info
    }
    property int icon: MessageDialog.Icon.None
    property alias buttonModel: dialogButtonBox.model
    property bool removeCloseButton: false
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowSystemMenuHint | (removeCloseButton? 0: Qt.WindowCloseButtonHint)
    modality: Qt.ApplicationModal
    color: Colors.background
    height: icon.height + dialogButtonBox.height + row.padding * 3
    minimumHeight: row.height + dialogButtonBox.height + row.padding
    width: Math.max(dialogButtonBox.contentWidth + dialogButtonBox.padding * 2, row.width)
    minimumWidth: row.width
    x: (screen.width - width) / 2
    y: (screen.height - height) / 2
    property string message: "Message Text"
    title: "Title of dialog"
    signal clicked(buttonIndex: int)
    onClicked: (buttonIndex) => {
        close();
    }
    signal focusButton(buttonIndex: int)
    onFocusButton: (buttonIndex) => {
        dialogButtonBox.focusButton(buttonIndex);
    }
    Row {
        id: row
        padding: 10
        spacing: 10
        Rectangle {
            id: iconRect
            width: icon == MessageDialog.Icon.None? 0: 64
            height: width
            color: "transparent"
            Image {
                anchors.fill: parent
                source: root.icon == MessageDialog.Icon.Question? "images/question.png":
                        root.icon == MessageDialog.Icon.Warning?  "images/warning.png":
                        root.icon == MessageDialog.Icon.Error?    "images/error.png":
                        root.icon == MessageDialog.Icon.Check?    "images/check.png":
                        root.icon == MessageDialog.Icon.Info?     "images/info.png":
                                                                  ""
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }
        }
        Label {
            anchors.verticalCenter: parent.verticalCenter
            text: message
            color: Colors.content
            textFormat: Text.RichText
            onLinkActivated: (link) => {
                Qt.openUrlExternally(link);
            }
        }
    }
    DialogButtons {
        id: dialogButtonBox
        anchors.top: row.bottom
        anchors.right: parent.right
        anchors.rightMargin: 10
        model: MessageDialogButtonBoxModel {
            Component.onCompleted: {
                append(MessageDialogButtonBoxModel.ButtonRoleOk, "");
            }
        }
        onButtonClicked: (buttonIndex) => {
            root.clicked(buttonIndex);
        }
    }
}
