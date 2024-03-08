import QtQml
import QtQuick
import QtQuick.Controls as QC

import YADAW.Models

Rectangle {
    id: root
    color: Colors.background
    clip: true

    property alias inputModel: inputChannels.model
    property alias outputModel: outputChannels.model
    property alias channelsModel: channels.model
    property alias showIO: ioButton.checked
    property alias showInsert: insertButton.checked
    property alias showSend: sendButton.checked
    property alias showFader: faderButton.checked
    property alias showInputBus: showInputBusButton.checked
    property alias showOutputBus: showOutputBusButton.checked

    property Window pluginSelectorWindow: null

    signal insertTrack(position: int, type: int) // AddTrackWindow.TrackType

    function appendTrack(type: int) { // AddTrackWindow.TrackType
        insertTrack(channels.count, type);
    }

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
        width: Math.min(contentWidth, 400)
        height: root.height
        orientation: Qt.Horizontal
        boundsBehavior: ListView.StopAtBounds
        clip: true
        delegate: Row {
            width: inputMixerChannel.width + inputMixerChannelBorder.width
            MixerChannel {
                id: inputMixerChannel
                height: root.height
                inputModel: 0
                outputModel: 0
                insertModel: mclm_inserts
                inputAvailable: false
                outputAvailable: false
                channelColor: mclm_color
                name: mclm_name
                showIO: root.showIO
                hasInstrumentSlot: false
                showInsertSlot: root.showInsert
                showSendSlot: root.showSend
                showFader: root.showFader
                pluginSelectorWindow: root.pluginSelectorWindow
                mute: mclm_mute
                onSetMute: (newMute) => {
                    mclm_mute = newMute;
                }
            }
            Rectangle {
                id: inputMixerChannelBorder
                width: 1
                height: root.height
                color: Colors.secondaryBorder
            }
        }
        Rectangle {
            anchors.right: parent.right
            width: 1
            height: root.height
            color: Colors.secondaryBorder
        }
    }
    ListView {
        id: channels
        anchors.left: root.showInputBus? inputChannels.right: leftBar.right
        anchors.leftMargin: root.showInputBus? 5: -1
        anchors.right: root.showOutputBus? outputChannels.left: parent.right
        anchors.rightMargin: root.showOutputBus? 5: -1
        clip: true
        height: root.height
        orientation: Qt.Horizontal
        boundsBehavior: ListView.StopAtBounds
        property int borderWidth: 1
        spacing: -1 * borderWidth
        delegate: Row {
            id: channelRow
            Rectangle {
                id: channelLeftBorder
                width: channels.borderWidth
                height: root.height
                color: Colors.secondaryBorder
            }
            MixerChannel {
                id: mixerChannel
                height: root.height
                inputModel: 0
                outputModel: 0
                insertModel: mclm_inserts
                inputAvailable:
                    mclm_channel_type == IMixerChannelListModel.ChannelTypeAudio
                    || mclm_channel_type == IMixerChannelListModel.ChannelTypeInstrument
                outputAvailable: true
                channelColor: mclm_color
                name: mclm_name
                showIO: root.showIO
                hasInstrumentSlot: mclm_channel_type == IMixerChannelListModel.ChannelTypeInstrument
                hasInstrument: mclm_instrument_exist
                instrumentBypassed: mclm_instrument_exist? mclm_instrument_bypassed: false
                instrumentName: mclm_instrument_exist? mclm_instrument_name: ""
                instrumentHasUI: mclm_instrument_exist? mclm_instrument_has_ui: false
                instrumentWindowVisible: mclm_instrument_exist? mclm_instrument_window_visible: false
                instrumentGenericEditorVisible: mclm_instrument_exist? mclm_instrument_generic_editor_visible: false
                instrumentLatency: mclm_instrument_exist? mclm_instrument_latency: 0
                showInsertSlot: root.showInsert
                showSendSlot: root.showSend
                showFader: root.showFader
                pluginSelectorWindow: root.pluginSelectorWindow
                mute: mclm_mute
                onSetMute: (newMute) => {
                    mclm_mute = newMute;
                }
                onHasInstrumentChanged: {
                    if(hasInstrument) {
                        if(instrumentHasUI) {
                            instrumentWindowVisible = true;
                        }
                        else {
                            instrumentGenericEditorVisible = true;
                        }
                    }
                }
                onSetInstrument: (instrumentId) => {
                    channelsModel.setInstrument(index, instrumentId);
                    console.log(mclm_instrument_exist);
                }
                onInstrumentWindowVisibleChanged: {
                    console.log("onInstrumentWindowVisibleChanged", instrumentWindowVisible);
                    mclm_instrument_window_visible = instrumentWindowVisible;
                }
                onInstrumentGenericEditorVisibleChanged: {
                    console.log("onInstrumentGenericEditorVisibleChanged", instrumentGenericEditorVisible);
                    mclm_instrument_generic_editor_visible = instrumentGenericEditorVisible;
                }
            }
            Rectangle {
                id: channelRightBorder
                width: channels.borderWidth
                height: root.height
                color: Colors.secondaryBorder
            }
        }
        readonly property int footerWidth: 60
        footer: Item {
            width: channels.footerWidth
            height: parent.height
        }
        MouseArea {
            id: mixerChannelBlankArea
            anchors.left: channels.contentItem.right
            anchors.leftMargin: -1 * channels.footerWidth
            width: Math.max(channels.footerWidth,
                channels.width - (channels.contentWidth - channels.contentX) + channels.footerWidth
            )
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            acceptedButtons: Qt.RightButton
            Menu {
                id: mixerChannelBlankAreaOptions
                title: qsTr("Mixer Channel Blank Area Options")
                Menu {
                    id: appendTrackMenu
                    title: qsTr("&Append Track")
                    MenuItem {
                        text: qsTr("&Audio") + "..."
                        onTriggered: {
                            root.appendTrack(AddTrackWindow.TrackType.Audio);
                        }
                    }
                    MenuItem {
                        text: qsTr("&Instrument") + "..."
                        onTriggered: {
                            root.appendTrack(AddTrackWindow.TrackType.Instrument);
                        }
                    }
                    MenuItem {
                        text: qsTr("Audio &Effect") + "..."
                        onTriggered: {
                            root.appendTrack(AddTrackWindow.TrackType.AudioEffect);
                        }
                    }
                    MenuItem {
                        text: qsTr("Audio &Bus") + "..."
                        onTriggered: {
                            root.appendTrack(AddTrackWindow.TrackType.AudioBus);
                        }
                    }
                }
            }
            onClicked: {
                mixerChannelBlankAreaOptions.x = mouseX;
                mixerChannelBlankAreaOptions.y = mouseY;
                mixerChannelBlankAreaOptions.open();
            }
        }
    }
    ListView {
        id: outputChannels
        visible: showOutputBus
        anchors.right: root.right
        width: Math.min(contentWidth, 400)
        height: root.height
        orientation: Qt.Horizontal
        boundsBehavior: ListView.StopAtBounds
        clip: true
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
                insertModel: mclm_inserts
                inputAvailable: false
                outputAvailable: false
                channelColor: mclm_color
                name: mclm_name
                showIO: root.showIO
                hasInstrumentSlot: false
                showInsertSlot: root.showInsert
                showSendSlot: root.showSend
                showFader: root.showFader
                pluginSelectorWindow: root.pluginSelectorWindow
                mute: mclm_mute
                onSetMute: (newMute) => {
                    mclm_mute = newMute;
                }
            }
        }
        Rectangle {
            anchors.left: parent.left
            width: 1
            height: root.height
            color: Colors.secondaryBorder
        }
    }
}
