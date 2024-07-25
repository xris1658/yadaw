import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import YADAW.Models

Window {
    id: root
    flags: Qt.Dialog
    title: qsTr("Add Track")
    modality: Qt.WindowModal
    color: Colors.background

    width: contents.width
    height: contents.height

    property alias trackType: trackTypeTabBar.currentIndex

    property alias audioInputList: audioInputModel.sourceModel
    property alias audioOutputList: audioOutputModel.sourceModel
    property alias midiInputList: midiInputModel.sourceModel

    property alias name: nameField.text
    property alias placeholderName: nameField.placeholderText
    property alias channelConfig: channelConfigComboBox.currentValue
    property bool instrumentEnabled: false
    property int instrument
    property bool audioEffectEnabled: false
    property int audioEffect
    property alias midiInputEnabled: midiInputCheckBox.checked
    property alias midiInput: midiInputComboBox.currentValue
    property alias audioInputEnabled: audioInputCheckBox.checked
    // property alias audioInput: audioInputComboBox.currentValue
    property alias audioOutputEnabled: audioOutputCheckBox.checked
    // property alias audioOutput: audioOutputComboBox.currentValue
    property alias count: countField.value

    property Window audioIOSelectorWindow: null
    property Window pluginSelectorWindow: null

    signal accepted()

    property int position: 0

    enum TrackType {
        Audio,
        Instrument,
        AudioEffect,
        AudioBus
    }

    SortFilterProxyListModel {
        id: audioInputModel
    }
    SortFilterProxyListModel {
        id: audioOutputModel
    }
    SortFilterProxyListModel {
        id: midiInputModel
    }
    SortFilterProxyListModel {
        id: midiOutputModel
    }

    QtObject {
        id: impl
        property int spacing: 5
    }
    Connections {
        id: connectToPluginSelector
        target: pluginSelectorWindow
        function onCancelled() {
            instrumentComboBox.checked = false;
            audioEffectComboBox.checked = false;
        }
        function onAccepted() {
            if(instrumentComboBox.checked) {
                root.instrumentEnabled = true;
                instrumentComboBox.text = pluginSelectorWindow.pluginSelector.currentPluginName();
                root.instrument = pluginSelectorWindow.pluginSelector.currentPluginId();
            }
            if(audioEffectComboBox.checked) {
                root.audioEffectEnabled = true;
                audioEffectComboBox.text = pluginSelectorWindow.pluginSelector.currentPluginName();
                root.audioEffect = pluginSelectorWindow.pluginSelector.currentPluginId();
            }
        }
        function onResetted() {
            if(instrumentComboBox.checked) {
                root.instrumentEnabled = false;
                instrumentComboBox.text = instrumentComboBox.defaultText;
            }
            if(audioEffectComboBox.checked) {
                root.audioEffectEnabled = false;
                audioEffectComboBox.text = audioEffectComboBox.defaultText;
            }
        }
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
                        id: audioEffectButton
                        width: implicitWidth
                        text: qsTr("Audio Effect")
                        onClicked: {
                            root.trackType = TabBar.index;
                        }
                    }
                    TabButton {
                        id: audioBusButton
                        width: implicitWidth
                        text: qsTr("Audio Bus")
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
                            placeholderText: trackTypeTabBar.itemAt(root.trackType).text
                        }
                        Label {
                            id: channelConfigLabel
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Channel Config") + ":"
                            horizontalAlignment: Label.AlignRight
                            visible: true
                        }
                        ComboBox {
                            id: channelConfigComboBox
                            width: gridContainer.secondColumnWidth
                            model: Constants.channelConfigProperties
                            textRole: "name"
                            valueRole: "type"
                            visible: channelConfigLabel.visible
                            onCurrentValueChanged: {
                                audioInputModel.setValueOfFilter(
                                    IAudioBusConfigurationModel.ChannelConfig,
                                    currentValue
                                );
                                audioOutputModel.setValueOfFilter(
                                    IAudioBusConfigurationModel.ChannelConfig,
                                    currentValue
                                );
                            }
                        }
                        Label {
                            id: instrumentText
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Instrument") + ":"
                            visible: root.trackType === AddTrackWindow.TrackType.Instrument
                            horizontalAlignment: Text.AlignRight
                        }
                        ComboBoxButton {
                            id: instrumentComboBox
                            width: gridContainer.secondColumnWidth
                            visible: instrumentText.visible
                            readonly property string defaultText: "<i>" + qsTr("No Instrument") + "</i>"
                            text: defaultText
                            onClicked: {
                                locatePopupWindow(pluginSelectorWindow, height, 0);
                                pluginSelectorWindow.pluginSelector.pluginListProxyModel.setValueOfFilter(
                                    IPluginListModel.Type,
                                    IPluginListModel.Instrument
                                );
                                pluginSelectorWindow.transientParent = root;
                                pluginSelectorWindow.pluginSelector.enableReset = true;
                                pluginSelectorWindow.pluginSelector.replacing = true;
                                pluginSelectorWindow.showNormal();
                            }
                        }
                        Label {
                            id: audioEffectText
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Audio Effect") + ":"
                            visible: root.trackType === AddTrackWindow.TrackType.AudioEffect
                            Component.onCompleted: {
                                leftPadding = width - implicitWidth;
                            }
                            horizontalAlignment: Text.AlignRight
                        }
                        ComboBoxButton {
                            id: audioEffectComboBox
                            readonly property string defaultText: "<i>" + qsTr("No Audio Effect") + "</i>"
                            text: defaultText
                            width: gridContainer.secondColumnWidth
                            visible: audioEffectText.visible
                            onClicked: {
                                locatePopupWindow(pluginSelectorWindow, height, 0);
                                pluginSelectorWindow.pluginSelector.pluginListProxyModel.setValueOfFilter(
                                    IPluginListModel.Type,
                                    IPluginListModel.AudioEffect
                                );
                                pluginSelectorWindow.transientParent = root;
                                pluginSelectorWindow.pluginSelector.enableReset = true;
                                pluginSelectorWindow.pluginSelector.replacing = true;
                                pluginSelectorWindow.showNormal();
                            }
                        }
                        CheckBox {
                            id: midiInputCheckBox
                            width: gridContainer.firstColumnWidth
                            text: qsTr("MIDI Input") + ":"
                            checked: true
                            visible: root.trackType === AddTrackWindow.TrackType.Instrument
                            Component.onCompleted: {
                                leftPadding = width - implicitWidth;
                            }
                        }
                        ComboBox {
                            id: midiInputComboBox
                            width: gridContainer.secondColumnWidth
                            visible: midiInputCheckBox.visible
                            enabled: midiInputCheckBox.checked
                            model: midiInputModel
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
                        ComboBoxButton {
                            id: audioInputComboBoxButton
                            width: gridContainer.secondColumnWidth
                            visible: audioInputCheckBox.visible
                            enabled: audioInputCheckBox.checked
                            // model: audioInputModel
                            // textRole: "abcm_name"
                            onCheckedChanged: {
                                if(checked) {
                                    locatePopupWindow(audioIOSelectorWindow, height, 0);
                                    audioIOSelectorWindow.showNormal();
                                }
                            }
                        }
                        CheckBox {
                            id: audioOutputCheckBox
                            width: gridContainer.firstColumnWidth
                            text: qsTr("Audio Output") + ":"
                            checked: true
                            visible: root.trackType === AddTrackWindow.TrackType.Audio
                                || root.trackType === AddTrackWindow.TrackType.Instrument
                                || root.trackType === AddTrackWindow.TrackType.AudioEffect
                                || root.trackType === AddTrackWindow.TrackType.AudioBus
                            Component.onCompleted: {
                                leftPadding = width - implicitWidth;
                            }
                        }
                        ComboBoxButton {
                            id: audioOutputComboBoxButton
                            width: gridContainer.secondColumnWidth
                            visible: audioOutputCheckBox.visible
                            enabled: audioOutputCheckBox.checked
                            // model: audioOutputModel
                            // textRole: "abcm_name"
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
                            to: 100
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
    onVisibleChanged: {
        if(visible) {
            nameField.forceActiveFocus();
        }
    }
}
