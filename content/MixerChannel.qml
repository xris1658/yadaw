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
    property alias sendModel: sendList.model
    property alias name: nameLabel.text
    property bool showIO: true
    property bool showInstrumentSlot: true
    property bool showInsertSlot: true
    property bool showSendSlot: true
    property bool showFader: true
    property alias mute: muteButton.checked
    property alias decibelValue: volumeFader.decibelValue

    property Window audioIOSelectorWindow: null
    property Window pluginSelectorWindow: null
    property Window pluginRouteEditorWindow: null

    property alias instrumentSlotChecked: instrumentSlot.checked

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
        property bool appendingSend: false
        property int settingSendIndex: -1
    }
    Connections {
        id: connectToPluginSelector
        target: impl.usingPluginSelector? pluginSelectorWindow: null
        function onAccepted() {
            if(impl.replaceInstrument) {
                impl.usingPluginSelector = false;
                let pluginId = pluginSelectorWindow.pluginSelector.currentPluginId();
                if(root.setInstrument(pluginId)) {
                    if(root.instrumentHasUI) {
                        setInstrumentWindowVisible(true);
                    }
                    else {
                        setInstrumentGenericEditorVisible(true);
                    }
                }
            }
            else {
                if(!pluginSelectorWindow.pluginSelector.replacing) {
                    impl.usingPluginSelector = false;
                    let pluginId = pluginSelectorWindow.pluginSelector.currentPluginId();
                    if(insertModel.insert(impl.insertPosition, pluginId)) {
                        let index = insertModel.index(impl.insertPosition, 0);
                        insertModel.setData(
                            index,
                            true,
                            insertModel.data(index, IMixerChannelInsertListModel.HasUI) == true?
                                IMixerChannelInsertListModel.WindowVisible:
                                IMixerChannelInsertListModel.GenericEditorVisible
                        );
                    }
                }
                else {
                    impl.usingPluginSelector = false;
                    let pluginId = pluginSelectorWindow.pluginSelector.currentPluginId();
                    if(insertModel.replace(impl.insertPosition, pluginId)) {
                        let index = insertModel.index(impl.insertPosition, 0);
                        insertModel.setData(
                            index,
                            true,
                            insertModel.data(index, IMixerChannelInsertListModel.HasUI) == true?
                                IMixerChannelInsertListModel.WindowVisible:
                                IMixerChannelInsertListModel.GenericEditorVisible
                        );
                    }
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
                mclm_output = audioIOSelectorWindow.audioIOSelector.currentPosition;
                impl.selectingOutput = false;
            }
            if(impl.appendingSend) {
                mclm_sends.append(true, audioIOSelectorWindow.audioIOSelector.currentPosition);
                impl.appendingSend = false;
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
        Item {
            id: ioPlaceholder
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
                    text: mclm_input? mclm_input.completeName: undefined
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
                    text: mclm_output? mclm_output.completeName: undefined
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
        Item {
            id: instrumentPlaceholder
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
            Layout.preferredWidth: root.width
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: instrumentPlaceholder.visible
        }
        ColumnLayout {
            id: insertAndSendPlaceholder
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
            visible: root.showInsertSlot || root.showSendSlot
            spacing: 0
            Item {
                id: insertPlaceholder
                Layout.preferredWidth: root.width
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
            Item {
                id: sendPlaceholder
                Layout.preferredWidth: root.width
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
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: impl.padding
                    width: sendPlaceholder.width - impl.padding * 2
                    spacing: impl.padding
                    Layout.fillHeight: true
                    Layout.verticalStretchFactor: 1
                    delegate: SendButton {
                        id: sendButton
                        width: sendList.width
                        text: mcsm_destination.completeName
                        MouseArea {
                            anchors.fill: parent
                            anchors.leftMargin: root.leftInset
                            anchors.rightMargin: root.rightInset
                            anchors.topMargin: root.topInset
                            anchors.bottomMargin: parent.height - implicitContentHeight
                            acceptedButtons: Qt.RightButton
                            Menu {
                                id: sendButtonOptions
                                title: qsTr("Send Options")
                                MenuItem {
                                    text: qsTr("&Delete")
                                    onClicked: {
                                        sendButton.removeThis();
                                    }
                                }
                            }
                            onClicked: (mouse) => {
                                if(mouse.button === Qt.RightButton) {
                                    let mainWindowItem = EventReceiver.mainWindow.background;
                                    let initialCoordinate = sendButton.mapFromItem(mainWindowItem, 0, 0);
                                    sendButtonOptions.x = initialCoordinate.x;
                                    sendButtonOptions.y = initialCoordinate.y;
                                    sendButtonOptions.open();
                                    let coor = sendButton.mapToItem(mainWindowItem, 0, sendButton.implicitContentHeight);
                                    if (coor.y >= mainWindowItem.height) {
                                        sendButtonOptions.y = 0 - sendButtonOptions.height;
                                    } else {
                                        sendButtonOptions.y = sendButton.implicitContentHeight;
                                    }
                                    if (coor.x + sendButtonOptions.width >= mainWindowItem.width) {
                                        sendButtonOptions.x = sendButton.width - sendButtonOptions.width;
                                    } else {
                                        sendButtonOptions.x = 0;
                                    }
                                }
                            }
                        }
                        property int itemIndex: index
                        function removeThis() {
                            sendModel.remove(itemIndex, 1);
                        }
                    }
                    footer: Button {
                        topInset: sendList.count !== 0? impl.padding: 0
                        width: sendList.width
                        onClicked: {
                            locatePopupWindow(audioIOSelectorWindow, height + impl.padding, 0 - impl.padding);
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareInput = false;
                            audioIOSelectorWindow.audioIOSelector.showAudioHardwareOutput = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioGroupChannel = true;
                            audioIOSelectorWindow.audioIOSelector.showAudioEffectChannel = true;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxIn = true;
                            audioIOSelectorWindow.audioIOSelector.showPluginAuxOut = false;
                            audioIOSelectorWindow.showNormal();
                            impl.usingAudioIOSelector = true;
                            impl.appendingSend = true;
                        }
                    }
                }
            }
        }
        Rectangle {
            width: root.width
            Layout.preferredHeight: impl.borderWidth
            color: Colors.border
            visible: insertAndSendPlaceholder.visible
        }
        Item {
            id: controlButtonPlaceholder
            width: root.width
            Layout.preferredHeight: muteButton.height + monitorButton.height + paddingSlider.height + impl.padding * 4
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
                        id: monitorButton
                        width: (controlButtonPlaceholder.width - impl.padding * 5) / 4
                        topPadding: 1
                        bottomPadding: 1
                        enabled: mclm_monitor_exist && mclm_input
                        opacity: mclm_monitor_exist? 1: 0
                        checkable: true
                        checked: mclm_monitor_exist? mclm_monitor: false
                        onCheckedChanged: {
                            mclm_monitor = checked;
                        }
                        backgroundColor: (!enabled)?
                            Colors.background:
                            checked?
                                down?
                                    Qt.darker(Colors.monitorButtonBackground, 1.25):
                                    hovered?
                                        Qt.lighter(Colors.monitorButtonBackground, 1.25):
                                        Colors.monitorButtonBackground:
                                down?
                                    Colors.pressedControlBackground:
                                    hovered?
                                        Colors.mouseOverControlBackground:
                                        Colors.controlBackground
                        Item {
                            anchors.centerIn: parent
                            width: 16
                            height: 16
                            layer.enabled: true
                            layer.smooth: true
                            layer.textureSize: Qt.size(width * 2, height * 2)
                            HeadphoneIcon {
                                path.fillColor: monitorButton.contentItem.color
                                path.strokeWidth: 0
                                anchors.centerIn: parent
                                scale: 16 / originalWidth
                            }
                        }
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
                ProgressBarLikeSlider {
                    id: paddingSlider
                    width: controlButtonPlaceholder.width - impl.padding * 2
                    height: muteButton.height
                    from: -100
                    to: 100
                    Label {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        text: Math.round(paddingSlider.value)
                    }
                }
                ComboBoxButton {
                    id: invertPolarityButton
                    text: "\u2205"
                    width: controlButtonPlaceholder.width - impl.padding * 2
                    height: paddingSlider.height
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
                    NativePopup {
                        id: invertPolarityNativePopup
                        width: invertPolarityMenu.width
                        height: Math.min(invertPolarityMenu.implicitHeight, screen.desktopAvailableHeight)
                        Menu {
                            id: invertPolarityMenu
                            width: Math.max(invertPolarityButton.width, implicitWidth)
                            height: parent.height
                            visible: invertPolarityNativePopup.visible
                            onVisibleChanged: {
                                if(!visible) {
                                    invertPolarityNativePopup.hide();
                                }
                            }
                            MenuItem {
                                text: qsTr("Invert all")
                                minimumSpaceBetweenTextAndShortcut: 0
                                onClicked: {
                                    mclm_polarity_inverter.invertAll();
                                }
                            }
                            MenuItem {
                                text: qsTr("Invert none")
                                minimumSpaceBetweenTextAndShortcut: 0
                                onClicked: {
                                    mclm_polarity_inverter.revertAll();
                                }
                            }
                            MenuItem {
                                text: qsTr("Toggle all")
                                minimumSpaceBetweenTextAndShortcut: 0
                                onClicked: {
                                    mclm_polarity_inverter.toggleAll();
                                }
                            }
                            MenuSeparator {}
                            Repeater {
                                model: mclm_polarity_inverter
                                MenuItem {
                                    checkable: true
                                    checked: pim_inverted
                                    text: pim_channel_name
                                    minimumSpaceBetweenTextAndShortcut: 0
                                    onCheckedChanged: {
                                        pim_inverted = checked;
                                    }
                                }
                            }
                        }
                        Connections {
                            target: visible? EventReceiver.mainWindow.keyEventForwarder: null
                            function onKeysPressed(event: var) {
                                let accepted = false;
                                if(event.key == Qt.Key_Up
                                    || event.key == Qt.Key_Backtab
                                    || ((event.key == Qt.Key_Tab && event.modifiers == Qt.ShiftModifier))) {
                                    if(invertPolarityMenu.currentIndex <= 0) {
                                        invertPolarityMenu.currentIndex = invertPolarityMenu.count - 1;
                                    }
                                    else {
                                        --invertPolarityMenu.currentIndex;
                                    }
                                    accepted = true;
                                }
                                else if(event.key == Qt.Key_Down
                                    || ((event.key == Qt.Key_Tab && event.modifiers == Qt.NoModifier))) {
                                    if(invertPolarityMenu.currentIndex == -1 || invertPolarityMenu.currentIndex == invertPolarityMenu.count - 1) {
                                        invertPolarityMenu.currentIndex = 0;
                                    }
                                    else {
                                        ++invertPolarityMenu.currentIndex;
                                    }
                                    accepted = true;
                                }
                                else if(event.key == Qt.Key_Return) {
                                    if(invertPolarityMenu.currentIndex != -1) {
                                        invertPolarityMenu.itemAt(invertPolarityMenu.currentIndex).clicked();
                                        accepted = true;
                                    }
                                }
                                else if(event.key == Qt.Key_Escape) {
                                    invertPolarityNativePopup.hide();
                                    accepted = true;
                                }
                                event.accepted = accepted;
                            }
                        }
                        onVisibleChanged: {
                            if(!visible) {
                                let nativePopupEventFilterModel = Global.nativePopupEventFilterModel;
                                if(nativePopupEventFilterModel) {
                                    nativePopupEventFilterModel.remove(invertPolarityNativePopup);
                                }
                                invertPolarityButton.checked = false;
                            }
                        }
                        onMousePressedOutside: {
                            hide();
                        }
                    }
                    onCheckedChanged: {
                        if(checked) {
                            let globalPoint = mapToGlobal(0, 0);
                            invertPolarityNativePopup.locate(
                                Qt.rect(
                                    globalPoint.x, globalPoint.y, width, height
                                ),
                                Qt.Vertical
                            );
                            invertPolarityNativePopup.showWithoutActivating();
                            let nativePopupEventFilterModel = Global.nativePopupEventFilterModel;
                            nativePopupEventFilterModel.append(invertPolarityNativePopup, false);
                            EventReceiver.mainWindow.keyEventForwarder.startForwarding(invertPolarityButton);
                        }
                        else {
                            EventReceiver.mainWindow.keyEventForwarder.endForwarding();
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
        Item {
            id: faderAndMeterPlaceholder
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
            visible: root.showFader
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
                        id: volumeFader
                        orientation: Qt.Vertical
                        height: parent.height - impl.padding
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        value: 0.796875
                        property double decibelValue: 0
                        function getDecibelValue(position: double): double {
                            for(let i = scales.model.count - 1; i >= 1; --i) {
                                let fromPosition = scales.model.get(i - 1).position;
                                let fromValue = scales.model.get(i - 1).value;
                                let toPosition = scales.model.get(i).position;
                                let toValue = scales.model.get(i).value;
                                if(position > fromPosition && position <= toPosition) {
                                    return fromValue + (position - fromPosition) / (toPosition - fromPosition) * (toValue - fromValue);
                                }
                            }
                            return -1 * Infinity;
                        }
                        function getPosition(decibelValue: double): double {
                            let maxValue = scales.model.get(scales.model.count - 1).value;
                            if(decibelValue > maxValue) {
                                return scales.model.get(scales.model.count - 1).position;
                            }
                            let minValue = scales.model.get(0).value;
                            if(decibelValue <= minValue) {
                                return scales.model.get(0).position;
                            }
                            for(let i = scales.model.count - 1; i >= 1; --i) {
                                let fromPosition = scales.model.get(i - 1).position;
                                let fromValue = scales.model.get(i - 1).value;
                                let toPosition = scales.model.get(i).position;
                                let toValue = scales.model.get(i).value;
                                if(decibelValue > fromValue && decibelValue <= toValue) {
                                    return fromPosition + (decibelValue - fromValue) / (toValue - fromValue) * (toPosition - fromPosition);
                                }
                            }
                        }
                        onPressedChanged: {
                            mclm_editing_volume = pressed;
                        }
                        onPositionChanged: {
                            decibelValue = getDecibelValue(position);
                        }
                        onDecibelValueChanged: {
                            mclm_volume = decibelValue;
                        }
                        Component {
                            id: volumeFaderScale
                            Item {
                                anchors.horizontalCenter: volumeFader.horizontalCenter
                                property int scaleWidth: volumeFader.handleThickness / 2 - 1
                                width: volumeFader.handleThickness + 2 * 2
                                height: 1
                                y: volumeFader.handleLength / 2 + (1 - position) * (volumeFader.height - volumeFader.handleLength)
                                Rectangle {
                                    width: scaleWidth
                                    height: 1
                                    anchors.left: parent.left
                                    color: Colors.border
                                }
                                Rectangle {
                                    width: scaleWidth
                                    height: 1
                                    anchors.right: parent.right
                                    color: Colors.border
                                }
                                Label {
                                    anchors.right: parent.left
                                    anchors.rightMargin: 1
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: value > 0? 0: -1
                                    color: Colors.secondaryContent
                                    text: index == 0? "-\u221e": value > 0? "+" + value.toString(): value
                                    font.pointSize: Qt.application.font.pointSize * 0.75
                                }
                                Label {
                                    anchors.left: parent.right
                                    anchors.leftMargin: 1
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: value > 0? 0: -1
                                    color: Colors.secondaryContent
                                    text: index == 0? "-\u221e": value > 0? "+" + value.toString(): value
                                    font.pointSize: Qt.application.font.pointSize * 0.75
                                }
                            }
                        }
                        handle.z: 3
                        Repeater {
                            id: scales
                            z: 2
                            delegate: volumeFaderScale
                            model: Constants.volumeFaderScaleModel
                        }
                    }
                }
                Item {
                    id: meterPlaceholder
                    width: faderAndMeterPlaceholder.width / 2
                    height: faderAndMeterPlaceholder.height - volumeLabel.height - parent.rowSpacing
                    Rectangle {
                        color: "#000000"
                        width: 20
                        height: parent.height - impl.padding - volumeFader.handleLength
                        anchors.top: parent.top
                        anchors.topMargin: volumeFader.handleLength / 2 + 2
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                MouseArea {
                    width: faderAndMeterPlaceholder.width / 2
                    height: volumeLabel.height
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    Rectangle {
                        anchors.fill: parent
                        anchors.bottomMargin: impl.padding / 2
                        z: 1
                        color: Colors.mouseOverControlBackground
                        visible: parent.containsMouse
                    }
                    Label {
                        id: volumeLabel
                        z: 2
                        text: volumeFader.position == 0? "-\u221e":
                            volumeFader.decibelValue < 0.01 && volumeFader.decibelValue > 0.0?   "+0.00":
                            volumeFader.decibelValue < 0.0  && volumeFader.decibelValue > -0.01? "-0.00":
                            volumeFader.decibelValue < 0.1  && volumeFader.decibelValue > -0.1?  volumeFader.decibelValue.toPrecision(1):
                            volumeFader.decibelValue < 1.0  && volumeFader.decibelValue > -1.0?  volumeFader.decibelValue.toPrecision(2):
                            volumeFader.decibelValue.toPrecision(3)
                        width: faderAndMeterPlaceholder.width / 2
                        topPadding: 0
                        bottomPadding: impl.padding
                        horizontalAlignment: Label.AlignHCenter
                        verticalAlignment: Label.AlignVCenter
                        TextField {
                            id: volumeTextField
                            anchors.fill: parent
                            topPadding: 0
                            bottomPadding: impl.padding
                            horizontalAlignment: Label.AlignHCenter
                            verticalAlignment: Label.AlignVCenter
                            visible: false
                            enabled: visible
                            onAccepted: {
                                let parsed = parseFloat(volumeTextField.text);
                                if(!isNaN(parsed)) {
                                    mclm_editing_volume = true;
                                    volumeFader.value = volumeFader.getPosition(volumeTextField.text);
                                    mclm_editing_volume = false;
                                }
                                visible = false;
                            }
                            Keys.onEscapePressed: (event) => {
                                visible = false;
                            }
                        }
                    }
                    onDoubleClicked: {
                        volumeTextField.visible = true;
                        volumeTextField.text = volumeLabel.text;
                        volumeTextField.selectAll();
                        volumeTextField.forceActiveFocus();
                    }
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
