import QtQml
import QtQuick
import QtQuick.Controls as QC

Rectangle {
    id: root
    color: Colors.background
    clip: true

    property var inputModel: null
    property var outputModel: null
    property alias showIO: ioButton.checked
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
            anchors.topMargin: (parent.width - ioButton.width) / 2
            spacing: anchors.topMargin
            Button {
                id: ioButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                IOIcon {
                    path.fillColor: parent.contentItem.color
                    path.strokeWidth: 0
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                }
            }
            Button {
                id: insertButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                InsertIcon {
                    path.fillColor: parent.contentItem.color
                    path.strokeWidth: 0
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                }
            }
            Button {
                id: sendButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                SendIcon {
                    path.fillColor: parent.contentItem.color
                    path.strokeWidth: 0
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                }
            }
            Button {
                id: faderButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                FaderIcon {
                    path.fillColor: parent.contentItem.color
                    path.strokeWidth: 0
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
                }
            }
        }
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: (parent.width - ioButton.width) / 2
            spacing: anchors.bottomMargin
            Button {
                id: showInputBusButton
                checkable: true
                border.width: 0
                width: 16
                height: 16
                ShowLeftPaneIcon {
                    path.fillColor: parent.contentItem.color
                    path.strokeWidth: 0
                    anchors.centerIn: parent
                    scale: parent.width / originalWidth
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
                    path.fillColor: parent.contentItem.color
                    path.strokeWidth: 0
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
        visible: showInputBus
        anchors.left: leftBar.right
        width: contentWidth
        orientation: Qt.Horizontal
        model: inputModel
        delegate: Row {
            MixerChannel {
                id: mixerChannel
                height: root.height
                inputModel: 0
                outputModel: 0
                inputAvailable: false
                outputAvailable: false
                channelColor: Colors.controlBackground
                name: abcm_name
                showIO: root.showIO
                showInstrumentSlot: true
                showInsertSlot: root.showInsert
                showSendSlot: root.showSend
                showFader: root.showFader
            }
            Rectangle {
                width: 1
                height: root.height
                color: Colors.secondaryBorder
            }
        }
    }
    ListView {
        visible: showOutputBus
        anchors.right: root.right
        width: contentWidth
        orientation: Qt.Horizontal
        model: outputModel
        delegate: Row {
            Rectangle {
                width: 1
                height: root.height
                color: Colors.secondaryBorder
            }
            MixerChannel {
                id: mixerChannel
                height: root.height
                inputModel: 0
                outputModel: 0
                inputAvailable: false
                outputAvailable: false
                channelColor: Colors.controlBackground
                name: abcm_name
                showIO: root.showIO
                showInstrumentSlot: true
                showInsertSlot: root.showInsert
                showSendSlot: root.showSend
                showFader: root.showFader
            }
        }
    }
}
