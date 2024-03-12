import QtQml
import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts

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
    property alias inputModel: inputButton.model
    property alias outputModel: outputButton.model
    property alias insertModel: insertList.model
    property alias name: nameLabel.text
    property bool showIO: true
    property bool showInstrumentSlot: true
    property bool showInsertSlot: true
    property bool showSendSlot: true
    property bool showFader: true

    property Window pluginSelectorWindow: null

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
    }
    Connections {
        id: connectToPluginSelector
        target: impl.usingPluginSelector? pluginSelectorWindow.pluginSelector: null
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
                ComboBox {
                    id: inputButton
                    opacity: root.inputAvailable? 1: 0
                    width: ioPlaceholder.width - impl.padding * 2
                    valueRole: "abcm_name"
                    textRole: "abcm_name"
                    displayText: count == 0? qsTr("No Bus Available"): currentIndex == -1? qsTr("Select Bus..."): currentText == ""? qsTr("Bus") + " " + (currentIndex + 1): currentText
                    indicator.visible: count != 0
                    enabled: count != 0
                }
                ComboBox {
                    id: outputButton
                    opacity: root.outputAvailable? 1: 0
                    width: ioPlaceholder.width - impl.padding * 2
                    valueRole: "abcm_name"
                    textRole: "abcm_name"
                    displayText: count == 0? qsTr("No Bus Available"): currentIndex == -1? qsTr("Select Bus..."): currentText == ""? qsTr("Bus") + " " + (currentIndex + 1): currentText
                    indicator.visible: count != 0
                    enabled: count != 0
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
                        root.setInstrumentWindowVisible(!root.instrumentWindowVisible);
                    }
                    else {
                        root.setInstrumentGenericEditorVisible(!root.instrumentGenericEditorVisible);
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
                        }
                        MenuItem {
                            text: qsTr("&Replace...")
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
                            instrumentOptions.open();
                            instrumentOptions.x = 0;
                            instrumentOptions.y = height;
                            let mainWindowItem = EventReceiver.mainWindow.background;
                            let popupCoordinate = instrumentSlot.mapToItem(mainWindowItem, instrumentOptions.x, instrumentOptions.y);
                            if(popupCoordinate.y + instrumentOptions.height >= mainWindowItem.height) {
                                console.log(instrumentOptions.implicitHeight, instrumentOptions.height);
                                instrumentOptions.y = 0 - instrumentOptions.height;
                            }
                            if(popupCoordinate.x + instrumentOptions.width >= mainWindowItem.width) {
                                instrumentOptions.x = instrumentSlot.mapFromItem(mainWindowItem, mainWindowItem.width - instrumentOptions.width, popupCoordinate.y).x;
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
                    let windowCoordinate = mapToGlobal(0, height + impl.padding);
                    if(windowCoordinate.y + pluginSelectorWindow.height >= pluginSelectorWindow.screen.desktopAvailableHeight) {
                        windowCoordinate = instrumentButton.mapToGlobal(0, 0 - pluginSelectorWindow.height - impl.padding);
                    }
                    if(windowCoordinate.x + pluginSelectorWindow.width >= pluginSelectorWindow.screen.desktopAvailableWidth) {
                        windowCoordinate.x = pluginSelectorWindow.screen.desktopAvailableWidth - pluginSelectorWindow.width;
                    }
                    pluginSelectorWindow.x = windowCoordinate.x;
                    pluginSelectorWindow.y = windowCoordinate.y;
                    pluginSelectorWindow.pluginSelector.pluginListModel.setValueOfFilter(
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
                        let windowCoordinate = mixerInsertSlot.mapToGlobal(0, mixerInsertSlot.height + impl.padding);
                        if(windowCoordinate.y + pluginSelectorWindow.height >= pluginSelectorWindow.screen.desktopAvailableHeight) {
                            windowCoordinate = mixerInsertSlot.mapToGlobal(0, 0 - pluginSelectorWindow.height - impl.padding);
                        }
                        if(windowCoordinate.x + pluginSelectorWindow.width >= pluginSelectorWindow.screen.desktopAvailableWidth) {
                            windowCoordinate.x = pluginSelectorWindow.screen.desktopAvailableWidth - pluginSelectorWindow.width;
                        }
                        pluginSelectorWindow.x = windowCoordinate.x;
                        pluginSelectorWindow.y = windowCoordinate.y;
                        pluginSelectorWindow.pluginSelector.pluginListModel.setValueOfFilter(
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
                                insertSlotOptions.open();
                                insertSlotOptions.x = 0;
                                insertSlotOptions.y = height;
                                let mainWindowItem = EventReceiver.mainWindow.background;
                                let popupCoordinate = mixerInsertSlot.mapToItem(mainWindowItem, insertSlotOptions.x, insertSlotOptions.y);
                                if(popupCoordinate.y + insertSlotOptions.height >= mainWindowItem.height) {
                                    console.log(insertSlotOptions.implicitHeight, insertSlotOptions.height);
                                    insertSlotOptions.y = 0 - insertSlotOptions.height;
                                }
                                if(popupCoordinate.x + insertSlotOptions.width >= mainWindowItem.width) {
                                    insertSlotOptions.x = mixerInsertSlot.mapFromItem(mainWindowItem, mainWindowItem.width - insertSlotOptions.width, popupCoordinate.y).x;
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
                        let windowCoordinate = mapToGlobal(0, height + impl.padding);
                        if(windowCoordinate.y + pluginSelectorWindow.height >= pluginSelectorWindow.screen.desktopAvailableHeight) {
                            windowCoordinate = mapToGlobal(0, 0 - pluginSelectorWindow.height - impl.padding);
                        }
                        if(windowCoordinate.x + pluginSelectorWindow.width >= pluginSelectorWindow.screen.desktopAvailableWidth) {
                            windowCoordinate.x = pluginSelectorWindow.screen.desktopAvailableWidth - pluginSelectorWindow.width;
                        }
                        pluginSelectorWindow.x = windowCoordinate.x;
                        pluginSelectorWindow.y = windowCoordinate.y;
                        pluginSelectorWindow.pluginSelector.pluginListModel.setValueOfFilter(
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
