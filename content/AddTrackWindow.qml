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

    property alias audioInputList: audioInputComboBox.model
    property alias audioOutputList: audioOutputComboBox.model
    property alias midiInputList: midiInputComboBox.model
    property alias midiOutputList: midiOutputComboBox.model
    property alias instrumentList: instrumentComboBox.model
    property alias audioEffectList: audioEffectComboBox.model

    property alias name: nameField.text
    property alias channelConfig: channelConfigComboBox.currentValue
    property alias instrumentEnabled: instrumentCheckBox.checked
    property alias instrument: instrumentComboBox.currentValue
    property alias audioEffectEnabled: audioEffectCheckBox.checked
    property alias audioEffect: audioEffectComboBox.currentValue
    property alias midiInputEnabled: midiInputCheckBox.checked
    property alias midiInput: midiInputComboBox.currentValue
    property alias midiOutputEnabled: midiOutputCheckBox.checked
    property alias midiOutput: midiOutputComboBox.currentValue
    property alias audioInputEnabled: audioInputCheckBox.checked
    property alias audioInput: audioInputComboBox.currentValue
    property alias audioOutputEnabled: audioOutputCheckBox.checked
    property alias audioOutput: audioOutputComboBox.currentValue
    property alias count: countField.value

    signal accepted()

    property int position: 0

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
                        CheckBox {
                            id: instrumentCheckBox
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Instrument") + ":"
                            checked: true
                            visible: root.trackType === AddTrackWindow.TrackType.Instrument
                            Component.onCompleted: {
                                leftPadding = width - implicitWidth;
                            }
                        }
                        ComboBox {
                            id: instrumentComboBox
                            width: gridContainer.secondColumnWidth
                            visible: instrumentCheckBox.visible
                            enabled: instrumentCheckBox.checked
                            textRole: "plm_name"
                            valueRole: "plm_id"
                            // TODO: update display text
                        }
                        CheckBox {
                            id: audioEffectCheckBox
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Audio Effect") + ":"
                            checked: true
                            visible: root.trackType === AddTrackWindow.TrackType.AudioEffect
                            Component.onCompleted: {
                                leftPadding = width - implicitWidth;
                            }
                        }
                        ComboBox {
                            id: audioEffectComboBox
                            width: gridContainer.secondColumnWidth
                            visible: audioEffectCheckBox.visible
                            enabled: audioEffectCheckBox.checked
                            textRole: "plm_name"
                            valueRole: "plm_id"
                            // TODO: update display text
                        }
                        CheckBox {
                            id: midiInputCheckBox
                            width: gridContainer.firstColumnWidth
                            text: qsTr("MIDI Input") + ":"
                            checked: true
                            visible: root.trackType === AddTrackWindow.TrackType.Instrument
                                || root.trackType === AddTrackWindow.TrackType.MIDI
                            Component.onCompleted: {
                                leftPadding = width - implicitWidth;
                            }
                        }
                        ComboBox {
                            id: midiInputComboBox
                            width: gridContainer.secondColumnWidth
                            visible: midiInputCheckBox.visible
                            enabled: midiInputCheckBox.checked
                        }
                        CheckBox {
                            id: audioInputCheckBox
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Audio Input") + ":"
                            checked: true
                            visible: root.trackType === AddTrackWindow.TrackType.Audio
                            Component.onCompleted: {
                                leftPadding = width - implicitWidth;
                            }
                        }
                        ComboBox {
                            id: audioInputComboBox
                            width: gridContainer.secondColumnWidth
                            visible: audioInputCheckBox.visible
                            enabled: audioInputCheckBox.checked
                            textRole: "abcm_name"
                        }
                        CheckBox {
                            id: midiOutputCheckBox
                            width: gridContainer.firstColumnWidth
                            text: qsTr("MIDI Output") + ":"
                            checked: true
                            visible: root.trackType === AddTrackWindow.TrackType.MIDI
                            Component.onCompleted: {
                                leftPadding = width - implicitWidth;
                            }
                        }
                        ComboBox {
                            id: midiOutputComboBox
                            width: gridContainer.secondColumnWidth
                            visible: midiOutputCheckBox.visible
                            enabled: midiOutputCheckBox.checked
                        }
                        CheckBox {
                            id: audioOutputCheckBox
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Audio Output") + ":"
                            checked: true
                            visible: root.trackType === AddTrackWindow.TrackType.Audio
                                || root.trackType === AddTrackWindow.TrackType.Instrument
                                || root.trackType === AddTrackWindow.TrackType.AudioEffect
                            Component.onCompleted: {
                                leftPadding = width - implicitWidth;
                            }
                        }
                        ComboBox {
                            id: audioOutputComboBox
                            width: gridContainer.secondColumnWidth
                            visible: audioOutputCheckBox.visible
                            enabled: audioOutputCheckBox.checked
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

        Item {
            width: tabContainer.width
            height: buttons.height + impl.spacing
            Row {
                id: buttons
                anchors.right: parent.right
                anchors.rightMargin: impl.spacing
                anchors.top: parent.top
                spacing: tabContent.padding
                Button {
                    id: okButton
                    text: qsTr("&Add")
                    onClicked: {
                        root.accepted();
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
        sequence: "Ctrl+Tab"
        onActivated: {
            let index = trackType === trackTypeTabBar.count - 1? 0: trackType + 1;
            root.trackType = index;
        }
    }
    Shortcut {
        context: Qt.WindowShortcut
        sequence: "Ctrl+Shift+Tab"
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
