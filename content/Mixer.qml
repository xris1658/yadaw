import QtQml
import QtQuick
import QtQuick.Controls as QC

Rectangle {
    id: root
    color: Colors.background
    clip: true

    property alias inputModel: inputChannels.model
    property alias outputModel: outputChannels.model
    property alias showIO: ioButton.checked
    property alias showInsert: insertButton.checked
    property alias showSend: sendButton.checked
    property alias showFader: faderButton.checked
    property alias showInputBus: showInputBusButton.checked
    property alias showOutputBus: showOutputBusButton.checked

    property PluginSelector pluginSelector: null

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
                checked: true
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
                checked: true
                border.width: 0
                width: 16
                height: 16
                enabled: showSend | showFader
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
                checked: true
                border.width: 0
                width: 16
                height: 16
                enabled: showInsert | showFader
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
                checked: true
                border.width: 0
                width: 16
                height: 16
                enabled: showInsert | showSend
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
        id: inputChannels
        visible: showInputBus
        anchors.left: leftBar.right
        width: contentWidth
        height: root.height
        orientation: Qt.Horizontal
        boundsBehavior: ListView.StopAtBounds
        delegate: Row {
            width: inputMixerChannel.width + inputMixerChannelBorder.width
            MixerChannel {
                id: inputMixerChannel
                height: root.height
                inputModel: 0
                outputModel: 0
                inputAvailable: false
                outputAvailable: false
                channelColor: mclm_color
                name: mclm_name
                showIO: root.showIO
                hasInstrument: false
                showInsertSlot: root.showInsert
                showSendSlot: root.showSend
                showFader: root.showFader
                pluginSelector: root.pluginSelector
            }
            Rectangle {
                id: inputMixerChannelBorder
                width: 1
                height: root.height
                color: Colors.secondaryBorder
            }
        }
    }
    ListView {
        id: outputChannels
        visible: showOutputBus
        anchors.right: root.right
        width: contentWidth
        height: root.height
        orientation: Qt.Horizontal
        boundsBehavior: ListView.StopAtBounds
        delegate: Row {
            width: outputMixerChannel.width + outputMixerChannelBorder.width
            Rectangle {
                id: outputMixerChannelBorder
                width: 1
                height: root.height
                color: Colors.secondaryBorder
            }
            MixerChannel {
                id: outputMixerChannel
                height: root.height
                inputModel: 0
                outputModel: 0
                inputAvailable: false
                outputAvailable: false
                channelColor: mclm_color
                name: mclm_name
                showIO: root.showIO
                hasInstrument: false
                showInsertSlot: root.showInsert
                showSendSlot: root.showSend
                showFader: root.showFader
                pluginSelector: root.pluginSelector
            }
        }
    }
}
