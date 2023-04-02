import QtQml
import QtQuick

Rectangle {
    id: root
    color: Colors.background

    clip: true
    Item {
        id: leftBar
        width: 20
        height: root.height
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: (parent.width - inputButton.width) / 2
            spacing: anchors.topMargin
            Button {
                id: inputButton
                border.width: 0
                width: 16
                height: 16
                InputIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                }
            }
            Button {
                border.width: 0
                width: 16
                height: 16
                OutputIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                }
            }
            Button {
                border.width: 0
                width: 16
                height: 16
                InsertIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                }
            }
            Button {
                border.width: 0
                width: 16
                height: 16
                SendIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                }
            }
            Button {
                border.width: 0
                width: 16
                height: 16
                FaderIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                }
            }
        }

        Rectangle {
            border.width: 0
            anchors.right: parent.right
            width: 1
            height: parent.height
            color: Colors.controlBorder
        }
    }
    ListView {
        anchors.left: leftBar.right
        orientation: Qt.Horizontal
        model: 1
        delegate: Row {
            MixerChannel {
                height: root.height
            }
            Rectangle {
                width: 1
                height: root.height
                color: Colors.controlBorder
            }
        }
    }
}
