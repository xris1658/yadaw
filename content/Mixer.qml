import QtQml
import QtQuick

Rectangle {
    id: root
    color: Colors.background
    clip: true

    property var inputModel: null
    property var outputModel: null
    property alias showInput: inputButton.checked
    property alias showOutput: outputButton.checked
    property alias showInsert: insertButton.checked
    property alias showSend: sendButton.checked
    property alias showFader: faderButton.checked
    property alias showInputBus: showInputBusButton.checked
    property alias showOutputBus: showOutputBusButton.checked
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
                checkable: true
                border.width: 0
                width: 16
                height: 16
                InputIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                    path.fillColor: parent.checked? Colors.content: Colors.secondaryContent
                }
            }
            Button {
                id: outputButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                OutputIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                    path.fillColor: parent.checked? Colors.content: Colors.secondaryContent
                }
            }
            Button {
                id: insertButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                InsertIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                    path.fillColor: parent.checked? Colors.content: Colors.secondaryContent
                }
            }
            Button {
                id: sendButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                SendIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                    path.fillColor: parent.checked? Colors.content: Colors.secondaryContent
                }
            }
            Button {
                id: faderButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                FaderIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                    path.fillColor: parent.checked? Colors.content: Colors.secondaryContent
                }
            }
        }
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: (parent.width - inputButton.width) / 2
            spacing: anchors.bottomMargin
            Button {
                id: showInputBusButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                ShowLeftPaneIcon {
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                    path.fillColor: parent.checked? Colors.content: Colors.secondaryContent
                }
            }
            Button {
                id: showOutputBusButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                ShowLeftPaneIcon {
                    rotation: 180
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                    path.fillColor: parent.checked? Colors.content: Colors.secondaryContent
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
                id: mixerChannel
                height: root.height
                inputModel: root.inputModel
                outputModel: root.outputModel
            }
            Rectangle {
                width: 1
                height: root.height
                color: Colors.secondaryBorder
            }
        }
    }
}
