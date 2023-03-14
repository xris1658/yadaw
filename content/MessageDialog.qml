import QtQuick
import QtQuick.Controls as QC
import QtQuick.Window

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
    flags: Qt.Dialog
    modality: Qt.ApplicationModal
    color: Colors.background
    height: icon.height + dialogButtonBox.height + row.padding * 2
    minimumHeight: row.height + dialogButtonBox.height
    width: Math.max(dialogButtonBox.contentWidth + dialogButtonBox.padding * 2, row.width)
    minimumWidth: row.width
    x: (screen.width - width) / 2
    y: (screen.height - height) / 2
    property string message: "Message Text"
    property alias standardButtons: dialogButtonBox.standardButtons
    signal accepted()
    title: "Title of dialog"
    signal clicked(button: QC.AbstractButton)
    onClicked: (button) => {
        //
    }
    property QC.DialogButtonBox dialogButtonBox: dialogButtonBox
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
        }
    }
    QC.DialogButtonBox {
        id: dialogButtonBox
        background: Rectangle {
            color: Colors.background
        }
        alignment: Qt.AlignRight
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: parent.width
        spacing: 5
        padding: 5
        onClicked: {
            root.close();
        }
        delegate: Button {
        }
    }
    Component.onCompleted: {
    }
}
