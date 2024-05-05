import QtQml
import QtQuick

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

    property PluginRouteEditorWindow pluginRouteEditorWindow: null

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
                pluginRouteEditorWindow: root.pluginRouteEditorWindow
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
                instrumentSlotChecked: mclm_instrument_exist? mclm_instrument_window_visible || mclm_instrument_generic_editor_visible: false
                instrumentAudioInputs: mclm_instrument_audio_inputs
                instrumentAudioOutputs: mclm_instrument_audio_outputs
                showInsertSlot: root.showInsert
                showSendSlot: root.showSend
                showFader: root.showFader
                pluginSelectorWindow: root.pluginSelectorWindow
                pluginRouteEditorWindow: root.pluginRouteEditorWindow
                onInstrumentBypassedChanged: {
                    mclm_instrument_bypassed = instrumentBypassed;
                }
                onSetInstrument: (instrumentId) => {
                    channelsModel.setInstrument(index, instrumentId);
                    if(mclm_instrument_has_ui) {
                        setInstrumentWindowVisible(true);
                    }
                    else {
                        setInstrumentGenericEditorVisible(true);
                    }
                }
                onRemoveInstrument: {
                    channelsModel.removeInstrument(index);
                }
                onSetInstrumentWindowVisible: (visible) => {
                    mclm_instrument_window_visible = visible;
                    instrumentWindowVisible = mclm_instrument_window_visible;
                }
                onSetInstrumentGenericEditorVisible: (visible) => {
                    mclm_instrument_generic_editor_visible = visible;
                    instrumentGenericEditorVisible = mclm_instrument_generic_editor_visible;
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
                pluginRouteEditorWindow: root.pluginRouteEditorWindow
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
