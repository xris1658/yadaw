import QtQml
import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts

import YADAW.Entities
import YADAW.Models

Rectangle {
    id: root
    color: Colors.background
    property alias channelColor: infoPlaceholder.color
    property bool inputAvailable: true
    property bool outputAvailable: true
    property bool hasInstrumentSlot: true
    property bool hasInstrument
    property string instrumentName
    property alias instrumentBypassed: instrumentSlot.bypassed
    property int instrumentLatency
    property bool instrumentHasUI
    property bool instrumentWindowVisible
    property bool instrumentGenericEditorVisible
    property var instrumentAudioInputs: null
    property var instrumentAudioOutputs: null
    property alias insertModel: insertList.model
    property alias name: nameLabel.text
    property bool showIO: true
    property bool showInstrumentSlot: true
    property bool showInsertSlot: true
    property bool showSendSlot: true
    property bool showFader: true

    property Window audioIOSelectorWindow: null
    property Window pluginSelectorWindow: null
    property Window pluginRouteEditorWindow: null

    property bool mute: muteButton.checked
    property alias instrumentSlotChecked: instrumentSlot.checked
    signal setMute(newMute: bool)

    signal setInstrument(pluginId: int)
    signal removeInstrument()
    signal setInstrumentWindowVisible(visible: bool)
    signal setInstrumentGenericEditorVisible(visible: bool)

    width: 120
    height: 400

    QtObject {
        id: impl
        property bool replaceInstrument: false
        property int insertPosition: -1
        property bool replacing: false
        readonly property int padding: 3
        readonly property int borderWidth: 1
        property bool usingPluginSelector: false
        property bool usingAudioIOSelector: false
        property alias selectingInput: inputButton.checked
        property alias selectingOutput: outputButton.checked
    }
    Connections {
        id: connectToPluginSelector
        target: impl.usingPluginSelector? pluginSelectorWindow: null
        function onAccepted() {
            if(impl.replaceInstrument) {
                impl.usingPluginSelector = false;
                let pluginId = pluginSelectorWindow.pluginSelector.currentPluginId();
                root.setInstrument(pluginId);
            }
            else {
                if(!pluginSelectorWindow.pluginSelector.replacing) {
                    impl.usingPluginSelector = false;
                    let pluginId = pluginSelectorWindow.pluginSelector.currentPluginId();
                    insertModel.insert(impl.insertPosition, pluginId);
                }
                else {
                    impl.usingPluginSelector = false;
                    let pluginId = pluginSelectorWindow.pluginSelector.currentPluginId();
                    insertModel.replace(impl.insertPosition, pluginId);
                }
            }
        }
        function onCancelled() {
            impl.usingPluginSelector = false;
        }
    }
    Connections {
        id: connectToAudioIOSelector
        target: impl.usingAudioIOSelector? audioIOSelectorWindow: null
        function onAccepted() {
            if(impl.selectingInput) {
                mclm_input = audioIOSelectorWindow.audioIOSelector.currentPosition;
                impl.selectingInput = false;
            }
            if(impl.selectingOutput) {
                mclm_input = audioIOSelectorWindow.audioIOSelector.currentPosition;
                impl.selectingOutput = false;
            }
        }
        function onResetted() {
            if(impl.selectingInput) {
                mclm_input = null;
                impl.selectingInput = false;
            }
            if(impl.selectingOutput) {
                mclm_output = null;
                impl.selectingOutput = false;
            }
        }
        function onCancelled() {
            impl.usingAudioIOSelector = false;
            impl.selectingInput = false;
            impl.selectingOutput = false;
        }
    }
    ColumnLayout {
        width: root.width
        height: root.height
        spacing: 0
        Rectangle {
            id: ioPlaceholder
            color: "transparent"
            width: root.width
            Layout.preferredHeight: ioComboBoxColumn.height + impl.padding * 2
            visible: root.showIO
            Column {
                id: ioComboBoxColumn
                spacing: impl.padding
                anchors.centerIn: parent
                ComboBoxButton {
                    id: inputButton
                    enabled: root.inputAvailable
                    opacity: enabled? 1: 0
                    width: ioPlaceholder.width - impl.padding * 2
                    onCheckedChanged: {
                        if(checked) {
                            locatePopupWindow(audioIOSelectorWindow, height + impl.padding, 0 - impl.padding);
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareInput = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareOutput = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioGroupChannel = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioEffectChannel = false;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxIn = false;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxOut = true;
                            audioIOSelectorWindow.audioIOSelector.currentIndex = 0;
                            audioIOSelectorWindow.audioIOSelector.audioChannelConfig = mclm_channel_config;
                            audioIOSelectorWindow.showNormal();
                            impl.usingAudioIOSelector = true;
                            impl.selectingInput = true;
                        }
                    }
                }
                ComboBoxButton {
                    id: outputButton
                    enabled: root.outputAvailable
                    opacity: enabled? 1: 0
                    width: ioPlaceholder.width - impl.padding * 2
                    onCheckedChanged: {
                        if(checked) {
                            locatePopupWindow(audioIOSelectorWindow, height + impl.padding, 0 - impl.padding);
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareInput = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareOutput = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioGroupChannel = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioEffectChannel = true;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxIn = true;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxOut = false;
                            audioIOSelectorWindow.audioIOSelector.currentIndex = 1;
                            audioIOSelectorWindow.audioIOSelector.audioChannelConfig = mclm_channel_config;
                            audioIOSelectorWindow.showNormal();
                            impl.usingAudioIOSelector = true;
                            impl.selectingOutput = true;
                        }
                    }
                }
            }
        }
        Rectangle {
            width: root.width
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: ioPlaceholder.visible
        }
        Rectangle {
            id: instrumentPlaceholder
            color: "transparent"
            width: root.width
            Layout.preferredHeight: instrumentButton.height + impl.padding * 2
            visible: root.showInstrumentSlot
            MixerInsertSlot {
                id: instrumentSlot
                visible: root.hasInstrumentSlot && root.hasInstrument
                anchors.centerIn: parent
                width: parent.width - impl.padding * 2
                bypassed: root.instrumentBypassed
                text: root.instrumentName
                onClicked: {
                    if(root.instrumentHasUI) {
                        root.setInstrumentWindowVisible(checked);
                    }
                    else {
                        root.setInstrumentGenericEditorVisible(checked);
                    }
                }
                MouseArea {
                    id: instrumentMouseArea
                    anchors.fill: parent
                    anchors.leftMargin: root.leftInset
                    anchors.rightMargin: root.rightInset
                    anchors.topMargin: root.topInset
                    anchors.bottomMargin: root.bottomInset
                    acceptedButtons: Qt.RightButton
                    Menu {
                        id: instrumentOptions
                        title: qsTr("Instrument Options")
                        MenuItem {
                            id: instrumentLatencyMenuItem
                            text: qsTr("Latency: ") + root.instrumentLatency + " " + qsTr("samples")
                        }
                        MenuSeparator {}
                        MenuItem {
                            text: root.instrumentVisible?
                                qsTr("Hide &Plugin Editor"):
                                qsTr("Show &Plugin Editor")
                            enabled: root.instrumentHasUI
                            onClicked: {
                                root.setInstrumentWindowVisible(!root.instrumentWindowVisible);
                            }
                        }
                        MenuItem {
                            text: root.instrumentGenericEditorVisible?
                                qsTr("Hide &Generic Editor"):
                                qsTr("Show &Generic Editor")
                            onClicked: {
                                root.setInstrumentGenericEditorVisible(!root.instrumentGenericEditorVisible);
                            }
                        }
                        MenuItem {
                            text: qsTr("&Edit Route...")
                            onClicked: {
                                if(root.pluginRouteEditorWindow) {
                                    root.pluginRouteEditorWindow.showNormal();
                                    root.pluginRouteEditorWindow.pluginRouteEditor.inputRouteListModel = root.instrumentAudioInputs;
                                    root.pluginRouteEditorWindow.pluginRouteEditor.outputRouteListModel = root.instrumentAudioOutputs;
                                }
                            }
                        }
                        MenuItem {
                            text: qsTr("&Replace") + "..."
                        }
                        MenuItem {
                            text: qsTr("&Delete")
                            onClicked: {
                                root.removeInstrument();
                            }
                        }
                    }
                    onClicked: (mouse) => {
                        if(mouse.button === Qt.RightButton) {
                            let mainWindowItem = EventReceiver.mainWindow.background;
                            let initialCoordinate = instrumentSlot.mapFromItem(mainWindowItem, 0, 0);
                            instrumentOptions.x = initialCoordinate.x;
                            instrumentOptions.y = initialCoordinate.y;
                            instrumentOptions.open();
                            let coor = instrumentSlot.mapToItem(mainWindowItem, 0, instrumentSlot.height + instrumentOptions.height);
                            if(coor.y >= mainWindowItem.height) {
                                instrumentOptions.y = 0 - instrumentOptions.height;
                            }
                            else {
                                instrumentOptions.y = instrumentSlot.height;
                            }
                            if(coor.x + instrumentOptions.width >= mainWindowItem.width) {
                                instrumentOptions.x = instrumentSlot.width - instrumentOptions.width;
                            }
                            else {
                                instrumentOptions.x = 0;
                            }
                        }
                    }
                }
            }
            Button {
                id: instrumentButton
                visible: root.hasInstrumentSlot && (!root.hasInstrument)
                anchors.centerIn: parent
                width: parent.width - impl.padding * 2
                onClicked: {
                    impl.replaceInstrument = true;
                    impl.usingPluginSelector = true;
                    impl.insertPosition = -1;
                    locatePopupWindow(pluginSelectorWindow, height + impl.padding, 0 - impl.padding);
                    pluginSelectorWindow.pluginSelector.pluginListProxyModel.setValueOfFilter(
                        IPluginListModel.Type,
                        IPluginListModel.Instrument
                    );
                    pluginSelectorWindow.pluginSelector.enableReset = false;
                    pluginSelectorWindow.pluginSelector.replacing = true;
                    pluginSelectorWindow.transientParent = EventReceiver.mainWindow;
                    pluginSelectorWindow.showNormal();
                }
                MouseArea {
                    anchors.fill: parent
                    anchors.leftMargin: parent.leftInset
                    anchors.rightMargin: parent.rightInset
                    anchors.topMargin: parent.topInset
                    anchors.bottomMargin: parent.bottomInset
                    acceptedButtons: Qt.RightButton
                }
            }
        }
        Rectangle {
            width: root.width
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: instrumentPlaceholder.visible
        }
        Rectangle {
            id: insertPlaceholder
            color: "transparent"
            width: root.width
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
            visible: root.showInsertSlot
            clip: true
            Rectangle {
                id: insertHeader
                anchors.top: parent.top
                width: parent.width
                height: insertLabel.height + impl.padding
                color: Colors.border
                Label {
                    id: insertLabel
                    text: "INSERTS"
                    font.pointSize: Qt.application.font.pointSize * 0.8
                    anchors.centerIn: parent
                }
                Button {
                    id: bypassedButton
                    anchors.right: parent.right
                    width: height
                    height: parent.height
                    z: 2
                    property int contentDiameter: parent.height / 2
                    leftInset: (width - contentDiameter) / 2
                    rightInset: leftInset
                    topInset: (height - contentDiameter) / 2
                    bottomInset: topInset
                    background: Rectangle {
                        anchors.centerIn: parent
                        width: parent.contentDiameter
                        height: width
                        radius: width / 2
                        color: Colors.background
                        border.color: Colors.secondaryBackground
                    }
                }
            }
            ListView {
                id: insertList
                anchors.top: insertHeader.bottom
                anchors.topMargin: impl.padding
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: impl.padding
                width: insertPlaceholder.width - impl.padding * 2
                spacing: impl.padding
                delegate: MixerInsertSlot {
                    id: mixerInsertSlot
                    width: insertList.width
                    text: mcilm_name
                    bypassed: mcilm_bypassed
                    onBypassedChanged: {
                        mcilm_bypassed = bypassed;
                    }
                    property bool windowVisible: mcilm_window_visible
                    property bool genericEditorVisible: mcilm_generic_editor_visible
                    checked: windowVisible || genericEditorVisible
                    onClicked: {
                        if(!checked) {
                            mcilm_window_visible = false;
                            mcilm_generic_editor_visible = false;
                        }
                        else {
                            if(mcilm_has_ui) {
                                mcilm_window_visible = true;
                            }
                            else {
                                mcilm_generic_editor_visible = true;
                            }
                        }
                    }
                    property int itemIndex: index
                    function removeThis() {
                        insertModel.remove(itemIndex, 1);
                    }
                    function showPluginSelector(replacing: bool) {
                        impl.replaceInstrument = false;
                        impl.usingPluginSelector = true;
                        impl.insertPosition = index;
                        locatePopupWindow(pluginSelectorWindow, height + impl.padding, 0 - impl.padding);
                        pluginSelectorWindow.pluginSelector.pluginListProxyModel.setValueOfFilter(
                            IPluginListModel.Type,
                            IPluginListModel.AudioEffect
                        );
                        pluginSelectorWindow.pluginSelector.enableReset = false;
                        pluginSelectorWindow.pluginSelector.replacing = replacing;
                        pluginSelectorWindow.transientParent = EventReceiver.mainWindow;
                        pluginSelectorWindow.showNormal();
                    }
                    MouseArea {
                        anchors.fill: parent
                        anchors.leftMargin: root.leftInset
                        anchors.rightMargin: root.rightInset
                        anchors.topMargin: root.topInset
                        anchors.bottomMargin: root.bottomInset
                        acceptedButtons: Qt.RightButton
                        Menu {
                            id: insertSlotOptions
                            title: qsTr("Insert Slot Options")
                            MenuItem {
                                id: latencyMenuItem
                                text: qsTr("Latency: ") + mcilm_latency + " " + qsTr("samples")
                            }
                            MenuSeparator {}
                            MenuItem {
                                text: mixerInsertSlot.windowVisible?
                                    qsTr("Hide &Plugin Editor"):
                                    qsTr("Show &Plugin Editor")
                                enabled: mcilm_has_ui
                                onClicked: {
                                    mcilm_window_visible = !mcilm_window_visible;
                                }
                            }
                            MenuItem {
                                text: mixerInsertSlot.genericEditorVisible?
                                    qsTr("Hide &Generic Editor"):
                                    qsTr("Show &Generic Editor")
                                onClicked: {
                                    mcilm_generic_editor_visible = !mcilm_generic_editor_visible;
                                }
                            }
                            MenuItem {
                                text: qsTr("&Edit Route...")
                                onClicked: {
                                    if(root.pluginRouteEditorWindow) {
                                        root.pluginRouteEditorWindow.show();
                                        root.pluginRouteEditorWindow.pluginRouteEditor.inputRouteListModel = mcilm_audio_inputs;
                                        root.pluginRouteEditorWindow.pluginRouteEditor.outputRouteListModel = mcilm_audio_outputs;
                                    }
                                }
                            }
                            MenuItem {
                                text: qsTr("&Insert") + "..."
                                onClicked: {
                                    mixerInsertSlot.showPluginSelector(false);
                                }
                            }
                            MenuItem {
                                text: qsTr("&Replace") + "..."
                                onClicked: {
                                    mixerInsertSlot.showPluginSelector(true);
                                }
                            }
                            MenuItem {
                                text: qsTr("&Delete")
                                onClicked: {
                                    mixerInsertSlot.removeThis();
                                }
                            }
                        }
                        onClicked: (mouse) => {
                            if(mouse.button === Qt.RightButton) {
                                let mainWindowItem = EventReceiver.mainWindow.background;
                                let initialCoordinate = mixerInsertSlot.mapFromItem(mainWindowItem, 0, 0);
                                insertSlotOptions.x = initialCoordinate.x;
                                insertSlotOptions.y = initialCoordinate.y;
                                insertSlotOptions.open();
                                let coor = mixerInsertSlot.mapToItem(mainWindowItem, 0, mixerInsertSlot.height + insertSlotOptions.height);
                                if (coor.y >= mainWindowItem.height) {
                                    insertSlotOptions.y = 0 - insertSlotOptions.height;
                                } else {
                                    insertSlotOptions.y = mixerInsertSlot.height;
                                }
                                if (coor.x + insertSlotOptions.width >= mainWindowItem.width) {
                                    insertSlotOptions.x = mixerInsertSlot.width - insertSlotOptions.width;
                                } else {
                                    insertSlotOptions.x = 0;
                                }
                            }
                        }
                    }
                }
                footer: Button {
                    topInset: insertList.count !== 0? impl.padding: 0
                    width: insertList.width
                    onClicked: {
                        impl.replaceInstrument = false;
                        impl.usingPluginSelector = true;
                        impl.insertPosition = insertList.count;
                        locatePopupWindow(pluginSelectorWindow, height + impl.padding, 0 - impl.padding);
                        pluginSelectorWindow.pluginSelector.pluginListProxyModel.setValueOfFilter(
                            IPluginListModel.Type,
                            IPluginListModel.AudioEffect
                        );
                        pluginSelectorWindow.pluginSelector.enableReset = false;
                        pluginSelectorWindow.pluginSelector.replacing = false;
                        pluginSelectorWindow.transientParent = EventReceiver.mainWindow;
                        pluginSelectorWindow.showNormal();
                    }
                }
            }
        }
        Rectangle {
            width: root.width
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: insertPlaceholder.visible
        }
        Rectangle {
            id: sendPlaceholder
            color: "transparent"
            width: root.width
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
            visible: root.showSendSlot
            clip: true
            Rectangle {
                id: sendHeader
                anchors.top: parent.top
                width: parent.width
                height: sendLabel.height + impl.padding
                color: Colors.border
                Label {
                    id: sendLabel
                    text: "SENDS"
                    font.pointSize: Qt.application.font.pointSize * 0.8
                    anchors.centerIn: parent
                }
                Button {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: height
                    height: sendLabel.height
                    anchors.rightMargin: (parent.height - height) / 2
                    radius: height / 2
                }
            }
            ListView {
                id: sendList
                anchors.top: sendHeader.bottom
                anchors.topMargin: impl.padding
                anchors.horizontalCenter: parent.horizontalCenter
                width: sendPlaceholder.width - impl.padding * 2
                spacing: impl.padding
                delegate: Button {
                    width: sendList.width
                }
                footer: Button {
                    width: sendList.width
                }
            }

            Column {
                anchors.top: sendHeader.bottom
                anchors.topMargin: impl.padding
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: impl.padding
                Repeater {
                    model: 1
                    Button {
                        width: sendPlaceholder.width - impl.padding * 2
                    }
                }
            }
        }
        Rectangle {
            width: root.width
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: sendPlaceholder.visible
        }
        Rectangle {
            id: controlButtonPlaceholder
            color: "transparent"
            width: root.width
            Layout.preferredHeight: muteButton.height + panningButton.height + impl.padding * 3
            Column {
                anchors.centerIn: parent
                spacing: impl.padding
                Row {
                    spacing: impl.padding
                    Button {
                        id: muteButton
                        text: "M"
                        width: (controlButtonPlaceholder.width - impl.padding * 5) / 4
                        topPadding: 1
                        bottomPadding: 1
                        checkable: true
                        backgroundColor: (!enabled)?
                            Colors.background:
                            checked?
                                down?
                                    Qt.darker(Colors.mutedButtonBackground, 1.25):
                                    hovered?
                                        Qt.lighter(Colors.mutedButtonBackground, 1.25):
                                        Colors.mutedButtonBackground:
                                down?
                                    Colors.pressedControlBackground:
                                    hovered?
                                        Colors.mouseOverControlBackground:
                                        Colors.controlBackground
                        onClicked: {
                            root.setMute(muteButton.checked);
                        }
                    }
                    Button {
                        id: soloButton
                        text: "S"
                        width: (controlButtonPlaceholder.width - impl.padding * 5) / 4
                        topPadding: 1
                        bottomPadding: 1
                        checkable: true
                        backgroundColor: (!enabled)?
                            Colors.background:
                            checked?
                                down?
                                    Qt.darker(Colors.soloedButtonBackground, 1.25):
                                    hovered?
                                        Qt.lighter(Colors.soloedButtonBackground, 1.25):
                                        Colors.soloedButtonBackground:
                                down?
                                    Colors.pressedControlBackground:
                                    hovered?
                                        Colors.mouseOverControlBackground:
                                        Colors.controlBackground
                    }
                    Button {
                        id: invertPolarityButton
                        text: "∅"
                        width: (controlButtonPlaceholder.width - impl.padding * 5) / 4
                        topPadding: 1
                        bottomPadding: 1
                        checkable: true
                        backgroundColor: (!enabled)?
                            Colors.background:
                            checked?
                                down?
                                    Qt.darker(Colors.invertedButtonBackground, 1.25):
                                    hovered?
                                        Qt.lighter(Colors.invertedButtonBackground, 1.25):
                                        Colors.invertedButtonBackground:
                                down?
                                    Colors.pressedControlBackground:
                                    hovered?
                                        Colors.mouseOverControlBackground:
                                        Colors.controlBackground
                    }
                    Button {
                        id: armRecordingButton
                        text: "R"
                        width: (controlButtonPlaceholder.width - impl.padding * 5) / 4
                        topPadding: 1
                        bottomPadding: 1
                        checkable: true
                        backgroundColor: (!enabled)?
                            Colors.background:
                            checked?
                                down?
                                    Qt.darker(Colors.armRecordingButtonBackground, 1.25):
                                    hovered?
                                        Qt.lighter(Colors.armRecordingButtonBackground, 1.25):
                                        Colors.armRecordingButtonBackground:
                                down?
                                    Colors.pressedControlBackground:
                                    hovered?
                                        Colors.mouseOverControlBackground:
                                        Colors.controlBackground
                    }
                }
                Item {
                    id: panningButton
                    width: root.width - impl.padding * 2
                    height: panningText.contentHeight
                    Item {
                        anchors.left: parent.left
                        width: parent.width / 2
                        height: parent.height
                        Row {
                            anchors.centerIn: parent
                            spacing: 5
                            Dial {
                                id: panningDial
                                diameter: panningText.contentHeight
                                from: -100
                                to: 100
                            }
                            Label {
                                id: panningText
                                width: panningTextMax.contentWidth
                                horizontalAlignment: Label.AlignHCenter
                                text: Math.floor(panningDial.value)
                                Label {
                                    id: panningTextMax
                                    text: "-100"
                                    visible: false
                                }
                            }
                        }
                    }
                    Item {
                        anchors.right: parent.right
                        width: parent.width / 2
                        height: parent.height
                        Button {
                            anchors.centerIn: parent
                            height: panningButton.height
                            text: "MONO"
                            checkable: true
                        }
                    }
                }

            }
        }
        Rectangle {
            width: root.width
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
        }
        Rectangle {
            id: faderAndMeterPlaceholder
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 2
            visible: root.showFader
            color: "transparent"
            width: root.width
            clip: true
            Grid {
                rows: 2
                columns: 2
                rowSpacing: impl.padding
                Item {
                    id: faderPlaceholder
                    width: faderAndMeterPlaceholder.width / 2
                    height: faderAndMeterPlaceholder.height - volumeLabel.height - parent.rowSpacing
                    Slider {
                        orientation: Qt.Vertical
                        height: parent.height - impl.padding
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Item {
                    id: meterPlaceholder
                    width: faderAndMeterPlaceholder.width / 2
                    height: faderAndMeterPlaceholder.height - volumeLabel.height - parent.rowSpacing
                    Rectangle {
                        color: "#003300"
                        width: 20
                        height: parent.height - impl.padding
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Label {
                    id: volumeLabel
                    text: "-6.00"
                    width: faderAndMeterPlaceholder.width / 2
                    topPadding: 0
                    bottomPadding: impl.padding
                    horizontalAlignment: Label.AlignHCenter
                    verticalAlignment: Label.AlignVCenter
                }
                Label {
                    id: levelLabel
                    text: "-6.00"
                    width: faderAndMeterPlaceholder.width / 2
                    topPadding: 0
                    bottomPadding: impl.padding
                    horizontalAlignment: Label.AlignHCenter
                    verticalAlignment: Label.AlignVCenter
                }
            }
        }
        Rectangle {
            id: infoPlaceholder
            width: root.width
            Layout.preferredHeight: nameLabel.height + impl.padding * 2
            Layout.fillHeight: false
            color: Colors.controlBackground
            Label {
                id: nameLabel
                anchors.centerIn: parent
                width: parent.width - impl.padding * 2
                horizontalAlignment: Label.AlignHCenter
                text: "Channel"
                color: Colors.isBright(infoPlaceholder.color)? "#000000": "#FFFFFF"
            }
        }
    }
}
