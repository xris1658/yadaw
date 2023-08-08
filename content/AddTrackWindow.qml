import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog
    title: qsTr("Add Track")
    modality: Qt.WindowModal
    color: Colors.background

    width: contents.width
    height: contents.height

    property alias trackType: trackTypeTabBar.currentIndex

    property var audioInputList: null
    property var audioOutputList: null
    property var midiInputList: null
    property var midiOutputList: null
    property var instrumentList: null
    property var audioEffectList: null

    enum TrackType {
        Audio,
        Instrument,
        MIDI,
        AudioEffect
    }

    QtObject {
        id: impl
        property int spacing: 5
    }
    Column {
        id: contents
        Item {
            id: tabContainer
            width: tabContent.width
            height: tabContent.height
            Column {
                id: tabContent
                padding: impl.spacing
                TabBar {
                    id: trackTypeTabBar
                    currentIndex: root.trackType
                    width: gridContainer.width
                    TabButton {
                        id: audioTrackButton
                        width: implicitWidth
                        text: qsTr("Audio")
                        onClicked: {
                            root.trackType = TabBar.index;
                        }
                    }
                    TabButton {
                        id: instrumentButton
                        width: implicitWidth
                        text: qsTr("Instrument")
                        onClicked: {
                            root.trackType = TabBar.index;
                        }
                    }
                    TabButton {
                        id: midiButton
                        width: implicitWidth
                        text: "MIDI"
                        onClicked: {
                            root.trackType = TabBar.index;
                        }
                    }
                    TabButton {
                        id: audioEffectButton
                        width: implicitWidth
                        text: qsTr("Audio Effect")
                        onClicked: {
                            root.trackType = TabBar.index;
                        }
                    }
                    onCurrentIndexChanged: {
                        nameField.forceActiveFocus();
                    }
                }
                Rectangle {
                    id: gridContainer
                    property int firstColumnWidth: 100
                    property int secondColumnWidth: 150
                    property int columnSpacing: impl.spacing
                    property int rowSpacing: impl.spacing
                    width: firstColumnWidth + secondColumnWidth + columnSpacing + rowSpacing * 2
                    height: grid.height + rowSpacing * 2
                    color: "transparent"
                    border.color: Colors.border
                    Grid {
                        id: grid
                        anchors.centerIn: parent
                        columns: 2
                        columnSpacing: gridContainer.columnSpacing
                        rowSpacing: gridContainer.rowSpacing
                        verticalItemAlignment: Grid.AlignVCenter
                        horizontalItemAlignment: Grid.AlignHCenter
                        Label {
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Name") + ":"
                            horizontalAlignment: Label.AlignRight
                        }
                        TextField {
                            id: nameField
                            width: gridContainer.secondColumnWidth
                        }
                        Label {
                            id: channelConfigLabel
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Channel Config") + ":"
                            horizontalAlignment: Label.AlignRight
                            visible: root.trackType === AddTrackWindow.TrackType.Audio
                                || root.trackType === AddTrackWindow.TrackType.AudioEffect
                        }
                        ComboBox {
                            id: channelConfigComboBox
                            width: gridContainer.secondColumnWidth
                            model: Constants.channelConfigProperties
                            textRole: "name"
                            valueRole: "type"
                            visible: channelConfigLabel.visible
                        }
                        Label {
                            id: instrumentLabel
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Instrument") + ":"
                            horizontalAlignment: Label.AlignRight
                            visible: root.trackType === AddTrackWindow.TrackType.Instrument
                        }
                        ComboBox {
                            id: instrumentComboBox
                            width: gridContainer.secondColumnWidth
                            visible: instrumentLabel.visible
                            model: root.instrumentList
                            textRole: "plm_name"
                            valueRole: "plm_id"
                            // TODO: update display text
                        }
                        Label {
                            id: audioEffectLabel
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Audio Effect") + ":"
                            horizontalAlignment: Label.AlignRight
                            visible: root.trackType === AddTrackWindow.TrackType.AudioEffect
                        }
                        ComboBox {
                            id: audioEffectComboBox
                            width: gridContainer.secondColumnWidth
                            visible: audioEffectLabel.visible
                            model: root.audioEffectList
                            textRole: "plm_name"
                            valueRole: "plm_id"
                            // TODO: update display text
                        }
                        Label {
                            id: midiInputLabel
                            width: gridContainer.firstColumnWidth
                            text: qsTr("MIDI Input") + ":"
                            horizontalAlignment: Label.AlignRight
                            visible: root.trackType === AddTrackWindow.TrackType.Instrument
                                || root.trackType === AddTrackWindow.TrackType.MIDI
                        }
                        ComboBox {
                            id: midiInputComboBox
                            width: gridContainer.secondColumnWidth
                            visible: midiInputLabel.visible
                            model: root.midiInputList
                        }
                        Label {
                            id: audioInputLabel
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Audio Input") + ":"
                            horizontalAlignment: Label.AlignRight
                            visible: root.trackType === AddTrackWindow.TrackType.Audio
                        }
                        ComboBox {
                            id: audioInputComboBox
                            width: gridContainer.secondColumnWidth
                            visible: audioInputLabel.visible
                            model: root.audioInputList
                            textRole: "abcm_name"
                        }
                        Label {
                            id: midiOutputLabel
                            width: gridContainer.firstColumnWidth
                            text: qsTr("MIDI Output") + ":"
                            horizontalAlignment: Label.AlignRight
                            visible: root.trackType === AddTrackWindow.TrackType.MIDI
                        }
                        ComboBox {
                            id: midiOutputComboBox
                            width: gridContainer.secondColumnWidth
                            visible: midiOutputLabel.visible
                            model: root.midiOutputList
                        }
                        Label {
                            id: audioOutputLabel
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Audio Output") + ":"
                            horizontalAlignment: Label.AlignRight
                            visible: root.trackType === AddTrackWindow.TrackType.Audio
                                || root.trackType === AddTrackWindow.TrackType.Instrument
                                || root.trackType === AddTrackWindow.TrackType.AudioEffect
                        }
                        ComboBox {
                            id: audioOutputComboBox
                            width: gridContainer.secondColumnWidth
                            visible: audioOutputLabel.visible
                            model: root.audioOutputList
                            textRole: "abcm_name"
                        }
                        Label {
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Count") + ":"
                            horizontalAlignment: Label.AlignRight
                        }
                        SpinBox {
                            id: countField
                            width: gridContainer.secondColumnWidth
                            height: implicitHeight
                            editable: true
                            from: 1
                        }
                        StackLayout.onIsCurrentItemChanged: {
                            if(StackLayout.isCurrentItem) {
                                audioNameField.forceActiveFocus();
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            width: tabContainer.width
            height: 1
            color: Colors.secondaryBorder
        }

        Rectangle {
            width: tabContainer.width
            height: buttons.height + impl.spacing * 2
            color: Colors.secondaryBackground
            Row {
                id: buttons
                anchors.right: parent.right
                anchors.rightMargin: impl.spacing
                anchors.verticalCenter: parent.verticalCenter
                spacing: tabContent.padding
                Button {
                    id: okButton
                    text: qsTr("&Add")
                    onClicked: {
                        // TODO
                        root.close();
                    }
                }
                Button {
                    id: cancelButton
                    text: Constants.cancelTextWithMnemonic
                    onClicked: {
                        root.close();
                    }
                }
            }
        }
    }
    Shortcut {
        context: Qt.WindowShortcut
        sequence: StandardKey.NextChild
        onActivated: {
            let index = trackType === trackTypeTabBar.count - 1? 0: trackType + 1;
            root.trackType = index;
        }
    }
    Shortcut {
        context: Qt.WindowShortcut
        sequence: StandardKey.PreviousChild
        onActivated: {
            let index = trackType === 0? trackTypeTabBar.count - 1: trackType - 1;
            root.trackType = index;
        }
    }
    Shortcut {
        context: Qt.WindowShortcut
        sequence: "Esc"
        onActivated: {
            cancelButton.clicked();
        }
    }

    Component.onCompleted: {
        nameField.forceActiveFocus();
    }
}
